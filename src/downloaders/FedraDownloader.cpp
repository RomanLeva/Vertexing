#include "FedraDownloader.hpp"

#include <TTree.h>
#include <TEventList.h>
#include <TFile.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TClonesArray.h>
#include <Rtypes.h>

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <iomanip>

// #include "fedra_classes/EdbSegP.h"
// #include "fedra_classes/MyClass.h"

// WARNING! FEDRA searches for linked_tracks.root and other parameters files in the project's root folder
// void readFedraLinkedTracks()
// {
//     // Int_t trackId;
//     // Int_t numberOfSegments;
//     Float_t chi2;
//     Float_t X, Y, Z;    // coordinates
//     Float_t tanX, tanY; // direction tangents

//     EdbDataProc *dproc = new EdbDataProc("lnk.def");
//     dproc->InitVolume(100, "");
//     EdbPVRec *pvr = dproc->PVR();
//     Int_t ntrk = pvr->Ntracks();

//     std::unique_ptr<TFile> myFile(TFile::Open("downloaded_tracks.root", "RECREATE"));
//     TTree *tree = new TTree("Tracks", "downloaded from linked_tracks.root");

//     // tree->Branch("trackId", &trackId, "trackId/I");
//     // tree->Branch("numberOfSegments", &numberOfSegments, "numberOfSegments/I");
//     tree->Branch("chi2", &chi2, "chi2/F");
//     tree->Branch("X", &X, "X/F");
//     tree->Branch("Y", &Y, "Y/F");
//     tree->Branch("Z", &Z, "Z/F");
//     tree->Branch("tanX", &tanX, "tanX/F");
//     tree->Branch("tanY", &tanY, "tanY/F");

//     for (Int_t i = 0; i < 300; i++)
//     {
//         auto edbTrack = pvr->GetTrack(i);

//         X = edbTrack->X();
//         Y = edbTrack->Y();
//         Z = edbTrack->Z();
//         tanX = edbTrack->TX();
//         tanY = edbTrack->TY();
//         chi2 = edbTrack->Chi2();

//         tree->Fill();
//         printf("X=%g ", X);
//         printf("Y=%g ", Y);
//         printf("Z=%g ", Z);
//         printf("tanX=%g ", tanX);
//         printf("tanY=%g ", tanY);
//         printf("\n");
//     }
//     tree->Write();

//     delete tree;
//     tree = nullptr;
//     printf("FEDRA linked_tracks.root file succesfully written to downloaded_tracks.root!\n");
// }

std::vector<Track> &FedraDownloader::downloadTracksFromFile(std::string fileName)
{
    TFile file(fileName.data());
    TTreeReader reader("Tracks", &file);

    TTreeReaderValue<Float_t> X(reader, "X");
    TTreeReaderValue<Float_t> Y(reader, "Y");
    TTreeReaderValue<Float_t> Z(reader, "Z");
    TTreeReaderValue<Float_t> tanX(reader, "tanX");
    TTreeReaderValue<Float_t> tanY(reader, "tanY");

    tracksVector.reserve(reader.GetEntries());

    u_long c = 0;
    while (reader.Next())
    {
        Track track(c, *X, *Y, *Z, *tanX, *tanY);

        for (size_t i = 0; i < 30; i++)
        {
            Segment seg(c, 111, 111, 111, 222, 222);
            track.addSegment(seg);
        }

        // if (c == 300000)
        // {
        //     break;
        // }

        tracksVector.push_back(track);
        c++;
    }
    file.Close();
    return tracksVector;
}

bool FedraDownloader::downloadTracksToFile(std::string fileName, std::vector<Track *> &tracks)
{
    return false;
}

std::vector<Vertex> &FedraDownloader::downloadVertexesFromFile(std::string fileName)
{
    return vertexesVector;
}

bool FedraDownloader::downloadVertexesToFile(std::string fileName, std::vector<Vertex *> &vertexes)
{
    if (fileName.find(".root") != std::string::npos)
    {
        auto file = TFile::Open(fileName.data(), "recreate");
        auto tree = new TTree("VertexTree", "VertexTree");
    }
    if (fileName.find(".txt") != std::string::npos)
    {
        std::ofstream outFile(fileName);
        for (auto vert : vertexes)
        {
            auto vXs1 = std::to_string(vert->getX());
            auto vXs2 = vXs1.substr(0, vXs1.find(".") + 3);
            auto vYs1 = std::to_string(vert->getY());
            auto vYs2 = vYs1.substr(0, vYs1.find(".") + 3);
            auto vZs1 = std::to_string(vert->getZ());
            auto vZs2 = vZs1.substr(0, vZs1.find(".") + 3);

            outFile << "1ry_vtx "
                    << "0 "
                    << "0 "
                    << "0 "
                    << "0 "
                    << std::to_string(vert->getIndex()) + " "
                    << vXs2 + " "
                    << vYs2 + " "
                    << vZs2 + " "
                    << "0 "
                    << "0 "
                    << std::to_string(vert->getDaughterTracksCount()) + " "
                    << "0 "
                    << "0 "
                    << "0 "
                    << "0 "
                    << std::endl;
            for (size_t t = 0; t < vert->getDaughterTracksCount(); t++)
            {
                auto tr = vert->getDaughterTrack(t);

                auto tXs1 = std::to_string(tr->getX());
                auto tXs2 = tXs1.substr(0, tXs1.find(".") + 3);
                auto tYs1 = std::to_string(tr->getY());
                auto tYs2 = tYs1.substr(0, tYs1.find(".") + 3);
                auto tTXs1 = std::to_string(tr->getTanX());
                auto tTXs2 = tTXs1.substr(0, tTXs1.find(".") + 3);
                auto tTYs1 = std::to_string(tr->getTanY());
                auto tTYs2 = tTYs1.substr(0, tTYs1.find(".") + 3);

                outFile << "1ry_trk "
                        << "0 "
                        << "0 "
                        << "0 "
                        << "0 "
                        << std::to_string(vert->getIndex()) + " "
                        << vXs2 + " "
                        << vYs2 + " "
                        << vZs2 + " "
                        << "0 "
                        << std::to_string(vert->getDaughterTracksCount()) + " "
                        << "0 "
                        << "0 "
                        << "0 "
                        << std::to_string(tr->getIndex()) + " "
                        << "0 "
                        << "0 "
                        << tXs2 + " "
                        << tYs2 + " "
                        << tTXs2 + " "
                        << tTYs2 + " "
                        << std::to_string(tr->getSegmentsCount()) + " "
                        << "0 "
                        << "0 "
                        << "0 "
                        << "0 "
                        << "0 "
                        << "0 "
                        << std::endl;
            }
            outFile << std::endl;
        }
        outFile.close();
        if (outFile.fail())
        {
            printf("====== ERROR in vertexes file writing! =======");
            return false;
        }
    }

    return true;
}
// std::vector<Track> &FedraDownloader::downloadLinkedTracksRoot()
// {
//     // Float_t chi2;
//     // Float_t X, Y, Z;    // coordinates
//     // Float_t tanX, tanY; // direction tangents

//     std::unique_ptr<TFile> file(TFile::Open("~/Vertexing/linked_tracks.root"));

//     TTree *tracks = (TTree *)file->Get("tracks");

//     Int_t trid = 0;
//     Int_t nseg = 0;
//     Int_t npl = 0;
//     Int_t n0 = 0;
//     Float_t xv = 0.;
//     Float_t yv = 0.;

//     // TEventList *lst = (TEventList *)gDirectory->GetList()->FindObject("lst");
//     // int nlst = lst->GetN();

//     TClonesArray *seg = new TClonesArray("EdbSegP", 60);
//     TClonesArray *segf = new TClonesArray("EdbSegP", 60);
//     EdbSegP *trk = 0;

//     tracks->SetBranchAddress("trid", &trid);
//     tracks->SetBranchAddress("nseg", &nseg);
//     tracks->SetBranchAddress("npl", &npl);
//     tracks->SetBranchAddress("n0", &n0);
//     tracks->SetBranchAddress("xv", &xv);
//     tracks->SetBranchAddress("yv", &yv);

//     tracks->SetBranchAddress("sf", &segf);
//     tracks->SetBranchAddress("s", &seg);
//     tracks->SetBranchAddress("t.", &trk);

//     Int_t ent = (Int_t)tracks->GetEntries();

//     for (Int_t i = 0; i < 10; i++)
//     {
//         // tracks->Print();
//         tracks->GetEntry(i); // here raises error about too large object
//         printf("entry=%d \n", i);

//         int lines = seg->GetEntries();
//         printf("seglines=%d \n", lines);
//         for (int s = 0; s < lines; s++)
//         {
//             // printf("seg=%d \n", s);
//             auto segment = seg->At(s);
//             // printf("segm",segment.)
//         }

//         Track track;
//         track.X = trk->eX;
//         track.Y = trk->eY;
//         track.Z = trk->eZ;
//         track.tanX = trk->eTX;
//         track.tanY = trk->eTY;

//         // eTrack=22018 eX=0 eY=0 eZ=2.17201e-43 eChi2=1.26117e-4
//         // eTrack=22022 eX=2.10195e-44 eY=0 eZ=5.23609e-41 eChi2=1.40127e-40
//         // eTrack=21967 eX=1.54143e-44 eY=0 eZ=1.29327e-40 eChi2=4.20387e-40
//         // printf("eTrack=%d ePID=%d eX=%g eY=%g eZ=%g eChi2=%g \n", trk->eTrack, trk->ePID, trk->eX, trk->eY, trk->eZ, trk->eChi2);
//         tracksVector.push_back(std::move(track));
//     }
//     printf("Downloaded Tracks vector of size=%lu from linked_tracks.root \n", tracksVector.size());
//     file->Close();
//     return tracksVector;
// }

// std::vector<Track> &FedraDownloader::TEST()
// {
//     MyClass c;
//     c.myVar = 3;

//     std::unique_ptr<TFile> writeFile(TFile::Open("~/Vertexing/test_file.root", "RECREATE"));

//     writeFile->WriteObjectAny(&c, "MyClass", "c");
//     writeFile->Close();

//     std::unique_ptr<TFile> readfile(TFile::Open("~/Vertexing/test_file.root"));
//     if (!readfile || readfile->IsZombie())
//     {
//         printf("error file");
//         exit(-1);
//     }
//     std::unique_ptr<MyClass> mc(readfile->Get<MyClass>("c"));
//     printf("HERE=%g \n", mc->myVar);

//     EdbSegP *p = new EdbSegP();
//     p->Chi2();

//     printf("Size of EdbPoint class: %li", sizeof(*p));

//     return tracksVector;
// }
