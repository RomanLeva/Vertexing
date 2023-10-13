#pragma once

#include "TF1.h"
#include <TH1F.h>
#include <TH1I.h>
#include <TH2F.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TApplication.h>
#include <TRootCanvas.h>
#include <TTree.h>

class Histograming
{
private:
    const std::string mcfilename = "~Vertexing/resources/MC_vertices_pl6-15_001-040.root";

public:
    void checkVertexesWithHistograms(std::vector<Vertex *> vertexes)
    {
        auto canvas = new TCanvas("DsTauVertexing", "DsTauVertexing", 0, 0, 900, 900);
        auto *rootCanvas = (TRootCanvas *)canvas->GetCanvasImp();
        canvas->Divide(1, 4);
        rootCanvas->Connect("CloseWindow()", "TApplication", gApplication, "Terminate()");

        canvas->cd(1);
        auto histVertex_X = new TH1F("Vertexes X coords", "vertexes X coords", 400, -10000, 10000);
        for (auto vertex : vertexes)
        {
            histVertex_X->Fill(vertex->getX());
        }
        histVertex_X->Draw();

        canvas->cd(2);
        auto histVertex_Y = new TH1F("Vertexes Y coords", "vertexes Y coords", 400, -10000, 10000);
        for (auto vertex : vertexes)
        {
            histVertex_Y->Fill(vertex->getY());
        }
        histVertex_Y->Draw();

        canvas->cd(3);
        auto histVertex_Z = new TH1F("Vertexes Z coords", "vertexes Z coords", 400, 0, 20000);
        for (auto vertex : vertexes)
        {
            histVertex_Z->Fill(vertex->getZ());
        }
        histVertex_Z->Draw();

        //TFile mcfile;
        //mcfile.cd("../resources/");
        //TTree *mctree = (TTree *)mcfile.Get("VertexTree");

        canvas->cd(4);
        auto histVertexMultiplicity = new TH1I("Vertexes multiplicity", "Vertexes multiplicity", 50, 0.5, 50.5);
        for (auto vertex : vertexes)
        {
            histVertexMultiplicity->Fill(vertex->getDaughterTracksCount());
        }
        histVertexMultiplicity->Draw();

        canvas->Modified();
        canvas->Update();

        // canvas->cd(5);
        // auto *histVertex_XY = new TH2F("Vertexes XY coords", "vertexes XY coords", 400, -10000, 10000, 400, -10000, 10000);
        // histVertex_XY->SetLineWidth(6);
        // for (auto vertex : vertexes)
        // {
        //     histVertex_XY->Fill(vertex->getX(), vertex->getY());
        // }
        // histVertex_XY->Draw();
    }

    Histograming(){};
    virtual ~Histograming(){};
};
