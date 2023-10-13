 void check_roman(const char* mcfilename = "../output/MC_vertices_pl6-15_001-040.root", const char* datafilename = "../output/vtx_tomoko.root", const char* newfilename ="../output/vtx_roman_20231004.root", const char* newfilename_1 ="../output/vtx_roman_20231004-1.root"){

	gROOT->SetStyle("Pub") ;
	TCanvas *c_posZ=new TCanvas("c_posZ", "c_posZ", 1024,680);
	c_posZ -> Divide(1,2);
	c_posZ->cd(1);
	
	TFile *mcfile = TFile::Open(mcfilename);
	TTree *mctree = (TTree*)mcfile->Get("VertexTree");
	
	TFile *datafile = TFile::Open(datafilename);
	TTree *datatree = (TTree*)datafile->Get("VertexTree");
	
	TFile *newfile = TFile::Open(newfilename);
	TTree *newtree = (TTree*)newfile->Get("VertexTree");
	
	TFile *newfile_1 = TFile::Open(newfilename_1);
	TTree *newtree_1 = (TTree*)newfile_1->Get("VertexTree");
      
      // fill true histos
      
      mctree->Draw("position[2]>>mc_posZ", "abs(position[0])<=7500&&abs(position[1])<=7500&&abs(position[2])>=2300&&abs(position[2])<=8000&&ndau>=4");//&&ndau>=4
      TH1D* mc_posZ = (TH1D*)gDirectory->Get("mc_posZ");
      
      datatree->Draw("position[2]>>data_posZ","abs(position[0])<=7500&&abs(position[1])<=7500&&abs(position[2])>=2300&&abs(position[2])<=8000","same");
      TH1D* data_posZ = (TH1D*)gDirectory->Get("data_posZ");
      
      newtree->Draw("position[2]>>new_posZ","abs(position[0])<=7500&&abs(position[1])<=7500&&abs(position[2])>=2300&&abs(position[2])<=8000","same");
      TH1D* new_posZ = (TH1D*)gDirectory->Get("new_posZ");
      
      newtree_1->Draw("position[2]>>new_posZ_1","abs(position[0])<=7500&&abs(position[1])<=7500&&abs(position[2])>=2300&&abs(position[2])<=8000","same");
      TH1D* new_posZ_1 = (TH1D*)gDirectory->Get("new_posZ_1");
      
       // set histo style
      mc_posZ->GetXaxis()->SetTitle("vtx z [mkm]");
      mc_posZ->GetYaxis()->SetTitle("N vtx");
      mc_posZ->SetLineColor(kBlue);
      data_posZ->SetLineColor(kGreen);
      new_posZ->SetLineColor(kRed);
      new_posZ_1->SetLineColor(kBlack);
      mc_posZ->SetStats(0);
      TLegend *leg = new TLegend(0.3,0.5,0.85,0.85);
      int ntrue = mc_posZ->Integral();
      int nrec = data_posZ->Integral();
      int nnew = new_posZ->Integral();
       int nnew_1 = new_posZ_1->Integral();
      leg->AddEntry(mc_posZ, Form("%i vtx - MC truth + rec-tracks, >=4 daughters with any length and angle", ntrue), "l");
      leg->AddEntry(data_posZ, Form("%i vtx - vtx Tomoko",nrec ), "l");
      leg->AddEntry(new_posZ, Form("%i vtx - vtx Roman",nnew ), "l");
      leg->AddEntry(new_posZ_1, Form("%i vtx - vtx Roman update",nnew_1 ), "l");
      leg->Draw("same");
      
      
      // fill reco histos
      
      c_posZ->cd(2);
      mctree->Draw("ndau>>mc_ndau", "abs(position[0])<=7500&&abs(position[1])<=7500&&abs(position[2])>=2300&&abs(position[2])<=8000&&ndau>=4");//&&ndau>=4
      TH1D* mc_ndau = (TH1D*)gDirectory->Get("mc_ndau");
      
      datatree->Draw("ndau>>data_ndau","abs(position[0])<=7500&&abs(position[1])<=7500&&abs(position[2])>=2300&&abs(position[2])<=8000","same");
      TH1D* data_ndau = (TH1D*)gDirectory->Get("data_ndau");
      
      newtree->Draw("ndau>>new_ndau","abs(position[0])<=7500&&abs(position[1])<=7500&&abs(position[2])>=2300&&abs(position[2])<=8000","same");
      TH1D* new_ndau = (TH1D*)gDirectory->Get("new_ndau");
      
      newtree_1->Draw("ndau>>new_ndau_1","abs(position[0])<=7500&&abs(position[1])<=7500&&abs(position[2])>=2300&&abs(position[2])<=8000","same");
      TH1D* new_ndau_1 = (TH1D*)gDirectory->Get("new_ndau_1");
      
      
    
      
      // set histo style
      mc_ndau->GetXaxis()->SetTitle("Multiplicity");
      mc_ndau->GetYaxis()->SetTitle("N vtx");
      mc_ndau->GetYaxis()->SetRangeUser(0,500);
      mc_ndau->SetLineColor(kBlue);
      data_ndau->SetLineColor(kGreen);
      new_ndau->SetLineColor(kRed);
      new_ndau_1->SetLineColor(kBlack);
      mc_ndau->SetStats(0);
      
      
      c_posZ->Print("../plots/mc_vtx_z_multip.png");
      c_posZ->Print("../plots/mc_vtx_z_multip.eps");
}
 

