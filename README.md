# Vertexing
DESCRIPTION OF THE OPERATION OF THE VERTEXING PROGRAM

The program consists of several parts, where each part is a separate folder with classes. There is also a separate simplest program needed that works with FEDRA and downloads from the linked_tracks.root file (details will be below).

Let's look at the launch and progress of the program, simultaneously describing some parts of the program. The first step is to use a separate program which will download tracks with using FEDRA framework due to imposibility of using FEDRA with CMake. The output is the downloaded_tracks.root file. This file is essentially the same tracks, but without unnecessary parameters, which made it possible to significantly reduce the file size and speed up the program accordingly. This is one of the main reasons for the high speed of the program - the less each object weighs, the faster it is pumped along the buses inside the computer and the more often it ends up in the cache memory. The downloaded tracks do not have segments, but only have the coordinates of the beginning and direction. Next, you need to manually transfer the downloaded_tracks.root file to the Vertexing/resources folder. After this, you can run the Vertexing program (in the folder Vertexing/build/DSTauVertexing.exe). Let's look at the progress of the program. The program starts classically in the main method of the MainClass.cpp class. An AppLogic object is created there, which represents a flow of program actions. The AppLogic::findVertexes() method is launched immediately. This method first creates a Detector object in RAM and downloads tracks from downloaded_tracks.root there. All processing now takes place only in RAM. It is worth noting that the detector is divided into spatial cells in which tracks, segments, and vertices will be stored. The cell size is calculated from the track density, which serves as another acceleration method. The cell should not be too large (when searching for a neighbor, extra tracks in the far corners of the cell are searched from a neighboring cell) and should not be too small (when searching for a neighbor, there should be no calls to empty cells). At the same time, when loading tracks, tracks with a small angle (base proton beam) will be sorted out. They will be loaded into the detector later, which speeds up processing a little, since there are no calls to direct tracks when searching for a neighbor. Next, an object of the VertexSearcher class is used, which is essentially a vertex search algorithm. An AppLogic object is a “user” of VertexSearcher, DetectorVolume, Downloader and others, which creates a detector divided into the appropriate number of cells, loads tracks into the detector, runs the algorithm, displays results and timing. This is done to separate the responsibilities of each of the classes, which will make it easy to expand the functionality.

Let's consider the vertex search algorithm in the VertexSearcher class. Each track is taken in turn, all neighbors are taken to it within the radius specified by the NEIGHBOR_TRACK_XY (or Z)_DISTANCE variables of about 1000 microns, where with each neighbor we look for the middle of the common perpendicular on the extensions of the tracks. This is a standard operation from analytical geometry. We look at the length of this perpendicular (TRACKS_PERPENDICULAR about 10 microns). Next, we look at how far this point is from the beginning of the tracks (the VERTEX_TO_TRACK_Z_DIST variable is about 1000 microns). With these variables we are cutting off un-physical vertices, since in fact any pair of straight lines will have some perpendicular and, accordingly, its middle, but the size of the perpendicular and the distance from the beginning of the tracks will show us that this is just a mathematical coincidence. Next, neighbors are taken to the found vertex (within a radius of NEIGHBOR_TRACK_XY (or Z)_DISTANCE about 1000 microns), the impact parameter between the vertex and the track is checked (IMPACT_PARAMETER about 15 microns), and if the track passes this CUT, then it joins the vertex and is eliminated from further search. That is, the track is marked with a special Boolean (track->isExcluded()), and in the future it can no longer be attached to another vertex and a common perpendicular will not be searched for with it. This greatly speeds up the program, since as the algorithm progresses, literally from the first pair of tracks that formed a vertex, most of the neighboring tracks immediately join it, they are excluded from the search, no new vertices are formed with them, etc. There is a problem that if two real vertices were formed next to each other, then such an algorithm can roughly attach all tracks, its own and others’, to some one vertex. It is necessary to clarify with a separate algorithm. It is worth noting that the algorithms for finding a common vertex, calculating the impact parameter and some others are performed using vector instructions on the CPU. This made it possible to speed up the program many times over, since there are a lot of searches between neighbors (and, accordingly, calculations), this was a bottleneck. Vector CPU instructions allow you to speed up typical mathematical vector operations, since such an operation inside the CPU uses special registers and is executed in one processor clock cycle, instead of classical operations where each vector element requires a separate CPU operation.
After finding vertices with attached tracks, we move each vertex so that the sum of its aiming parameters with each of its tracks is minimal. This is done by the ROOT TMinuit package. At the end of the algorithm, all vertices with less than 4 child tracks are deleted. Next, the vertices with their child tracks are uploaded to a file.
Let us remind you again - since each class is responsible for its own, and if you need to change the chain of actions of the program, then you need to change the methods in the AppLogic class, if you need to output to some other file format, then you need to change the downloadVertexesToFile method in the class descendant from IDownloader (in our In this case, this is the FedraDownloader class) You can easily override the IDownloader descendant with your own completely different methods, this will not affect other classes, since they do not depend on the implementation, but simply use the IDownloader interface. If you need to change the search algorithm, then this is VertexSearcher. And so on. You can implement downloading from Fedra in FedraDwonloader::downloadTracksFromFile if you manage to connect Fedra. That is, the idea is that each new functionality is added to the corresponding class and does not in any way affect the users of this class or the rest of the program.

Let's look at some notes on using the DetectorVolume object, which is a detector with cells.
This object is a storage of tracks, their segments, and vertices. The detector has a typical set of methods such as adding a track or vertex, removing a track or vertex, checking whether such a vertex exists at a certain coordinate point, searching for and taking a vertex at a certain point. And also taking all tracks or vertices around a certain point (this is what is used when searching for neighbors), counting all tracks or vertices in the detector and some other methods. When adding a track or vertex to the detector, it itself calculates the desired cell based on the coordinates of this track or vertex. This allows you to sort an arbitrary set of tracks or vertices into cells, which will then give an instantaneous search speed at a certain coordinate point. Each cell stores an array of tracks and an array of vertices. Ideally, for access speed, you should strive for a cell to have on average one object inside it. If the cell is large, then when requesting an object from it, a search will occur inside the array; if the cell is small, then such empty cells also take time to search for the desired object using them. The optimal cell size is calculated in CalculationAndAlgorithms::calculateCellSizeFromTracksCount. Next, when requesting, for example, all tracks within a certain radius, the detector returns an array of pointers to objects. Each such pointer cannot change the coordinates of the object! This is done so that the user cannot move a track or vertex from outside the detector, since when moving, the cell that should store the object may change, which will break the program. Therefore, for example, in an algorithm where we move a vertex by minimizing the sum of its impact parameters, each time we create a new moved vertex, add this new vertex to the detector, and be sure to delete the old vertex so that there are no duplicates (it is advisable to implement this method inside the detector using the UPDATE type in databases). This approach slows down the program almost imperceptibly, since there are quite a few vertices left at the output. And objects such as tracks or segments cannot, in principle, be moved anywhere.
There are some points in the program that may seem suboptimal. For example, creating a vector of pointers to vertices and passing this vector not by reference, but by value. But this does not require changes, since the compiler itself will optimize and remove intermediate objects.
