#ifndef CANVAS_EXTRAS_H
#define CANVAS_EXTRAS_H

#include "TCanvas.h"
#include "TPaletteAxis.h"
#include "TH2.h"
#include "TStyle.h"

TCanvas* custom_can_(TH1* h, std::string canname, int gx = 0, int gy = 0,
		     int histosize_x = 500, int histosize_y = 500,
		     int mar_left = 80, int mar_right = 20, int mar_top = 20, int mar_bottom = 60, 
		     int title_align = 33, float title_x = 1.0, float title_y = 1.0,
		     bool log=false) {
  if (std::string(h->GetTitle()).size()>0&&title_y==1.0) mar_top += 20;
  int titlefontsize = 32;
  int labelfontsize = 20;
  int yoffset_x = mar_left - titlefontsize - 4;
  int xoffset_y = mar_bottom - titlefontsize - 4;
  int zoffset_x = mar_right - titlefontsize - 4;
  int padsize_x = histosize_x + mar_left + mar_right;
  int padsize_y = histosize_y + mar_top + mar_bottom;
  int padsize = ((padsize_x<=padsize_y) ? padsize_x : padsize_y);
  float padratio_yx = (float)padsize_y/padsize_x > 1 ? 1 : (float)padsize_y/padsize_x;
  float padratio_xy = (float)padsize_x/padsize_y > 1 ? 1 : (float)padsize_x/padsize_y;
  Float_t xoffset = ((Float_t)xoffset_y/titlefontsize+0.5) * padratio_xy /1.6;
  Float_t yoffset = ((Float_t)yoffset_x/titlefontsize+0.5) * padratio_yx /1.6;
  Float_t zoffset = ((Float_t)zoffset_x/titlefontsize+0.5) * padratio_yx /1.6;
  Float_t titlesize = (Float_t)titlefontsize/padsize;
  Float_t labelsize = (Float_t)labelfontsize/padsize;
  gStyle->SetOptStat(0);
  if (std::string(h->GetTitle()).size()) {
    gStyle->SetOptTitle(1);
    gStyle->SetTitleH(titlefontsize/padsize);
    gStyle->SetTitleFontSize(titlesize*0.8);
    gStyle->SetTitleBorderSize(0);
    gStyle->SetTitleAlign(title_align);
    gStyle->SetTitleX(title_x);
    gStyle->SetTitleY(title_y);
  }
  h->SetTitleFont(42,"xyz");
  h->SetLabelFont(42,"xyz");
  h->SetTitleSize(titlesize,"xyz");
  h->SetLabelSize(labelsize,"xyz");
  h->GetXaxis()->SetTitleOffset(xoffset);
  h->GetYaxis()->SetTitleOffset(yoffset);
  h->GetZaxis()->SetTitleOffset(zoffset);
  h->GetYaxis()->SetDecimals(1);
  h->GetZaxis()->SetDecimals(1);
  TCanvas* canvas = new TCanvas(canname.c_str(), h->GetTitle(), padsize_x + 4, padsize_y + 26);
  TVirtualPad* pad = canvas->cd(1);
  pad->SetLeftMargin((Float_t)mar_left/padsize_x);
  pad->SetRightMargin((Float_t)mar_right/padsize_x);
  pad->SetTopMargin((Float_t)mar_top/padsize_y);
  pad->SetBottomMargin((Float_t)mar_bottom/padsize_y);
  canvas->SetGrid(gx,gy);
  if (log) canvas->SetLogy(1);
  return canvas;
}

TCanvas* custom_can_(TH2* h, std::string canname, int gx = 0, int gy = 0,
		     int histosize_x = 500, int histosize_y = 500, 
		     int mar_left = 80, int mar_right = 120, int mar_top = 20, int mar_bottom = 60, 
		     int title_align = 33, float title_x = 1.0, float title_y = 1.0,
		     std::string draw="COLZ", bool norm=false, bool log=false) {
  if (std::string(h->GetTitle()).size()>0&&title_y==1.0) mar_top += 20;
  int titlefontsize = 32;
  int labelfontsize = 20;
  int pal_offset_x = 5;
  int pal_width_x = 25;
  int xoffset_y = mar_bottom - titlefontsize - 4;
  int yoffset_x = mar_left - titlefontsize - 4;
  int zoffset_x = mar_right - pal_offset_x - pal_width_x - titlefontsize;
  int padsize_x = histosize_x + mar_left + mar_right;
  int padsize_y = histosize_y + mar_top + mar_bottom;
  int padsize = ((padsize_x<=padsize_y) ? padsize_x : padsize_y);
  float padratio_yx = (Float_t)padsize_y/padsize_x > 1 ? 1 : (Float_t)padsize_y/padsize_x;
  float padratio_xy = (Float_t)padsize_x/padsize_y > 1 ? 1 : (Float_t)padsize_x/padsize_y;
  Float_t xoffset = ((Float_t)xoffset_y/titlefontsize+0.5) * padratio_xy /1.6;
  Float_t yoffset = ((Float_t)yoffset_x/titlefontsize+0.5) * padratio_yx /1.6;
  Float_t zoffset = ((Float_t)zoffset_x/titlefontsize+0.5) * padratio_yx /1.6;
  Float_t titlesize = (Float_t)titlefontsize/padsize;
  Float_t labelsize = (Float_t)labelfontsize/padsize;
  h->SetTitleFont(42,"xyz");
  h->SetLabelFont(42,"xyz");
  h->SetTitleSize(titlesize,"xyz");
  h->SetLabelSize(labelsize,"xyz");
  h->GetXaxis()->SetTitleOffset(xoffset);
  h->GetYaxis()->SetTitleOffset(yoffset);
  h->GetZaxis()->SetTitleOffset(zoffset);
  h->GetZaxis()->RotateTitle(1);
  h->GetYaxis()->SetDecimals(1);
  h->GetZaxis()->SetDecimals(1);
  if (histosize_y<250) h->GetZaxis()->SetNdivisions(505);
  if (std::string(h->GetTitle()).size()) {
    gStyle->SetOptTitle(1);
    gStyle->SetTitleH(titlefontsize/padsize);
    gStyle->SetTitleFontSize(titlesize*0.8);
    gStyle->SetTitleBorderSize(0);
    gStyle->SetTitleAlign(title_align);
    gStyle->SetTitleX(title_x);
    gStyle->SetTitleY(title_y);
  }
  TCanvas* canvas = new TCanvas(canname.c_str(), h->GetTitle(), padsize_x + 4, padsize_y + 26);
  TVirtualPad* pad = canvas->cd(1);
  pad->SetLeftMargin((Float_t)mar_left/padsize_x);
  pad->SetRightMargin((Float_t)mar_right/padsize_x);
  pad->SetTopMargin((Float_t)mar_top/padsize_y);
  pad->SetBottomMargin((Float_t)mar_bottom/padsize_y);
  canvas->SetGrid(gx,gy);
  if (norm&&h->Integral()>0) h = (TH2D*)h->DrawNormalized(draw.c_str());
  else h->Draw(draw.c_str());
  if (h->Integral()>0&&draw=="COLZ") {
    gPad->Update();
    TPaletteAxis* palette = (TPaletteAxis*)h->GetListOfFunctions()->FindObject("palette");
    if (palette) {
      palette->SetX1NDC(1 - (Float_t)(mar_right - pal_offset_x)/padsize_x);
      palette->SetX2NDC(1 - (Float_t)(mar_right - pal_offset_x - pal_width_x)/padsize_x);
      palette->SetY1NDC((Float_t)mar_bottom/padsize_y);
      palette->SetY2NDC(1 - (Float_t)mar_top/padsize_y);
    }
  }
  if (log) canvas->SetLogz(1);
  return canvas;
}


void redesignCanvas(TCanvas* canvas, TH1* histogram)
{
  int gx = 0;
  int gy = 0;
  int histosize_x = 500;
  int histosize_y = 500;
  int mar_left = 80;
  int mar_right = 20;
  int mar_top = 20;
  int mar_bottom = 60;
  int title_align = 33;
  float title_x = 1.0;
  float title_y = 1.0;
  if (std::string(histogram -> GetTitle()).size()>0&&title_y==1.0) mar_top += 20;
  int titlefontsize = 32;
  int labelfontsize = 20;
  int yoffset_x = mar_left - titlefontsize - 4;
  int xoffset_y = mar_bottom - titlefontsize - 4;
  int zoffset_x = mar_right - titlefontsize - 4;
  int padsize_x = histosize_x + mar_left + mar_right;
  int padsize_y = histosize_y + mar_top + mar_bottom;
  int padsize = ((padsize_x<=padsize_y) ? padsize_x : padsize_y);
  float padratio_yx = (float)padsize_y/padsize_x > 1 ? 1 : (float)padsize_y/padsize_x;
  float padratio_xy = (float)padsize_x/padsize_y > 1 ? 1 : (float)padsize_x/padsize_y;
  Float_t xoffset = ((Float_t)xoffset_y/titlefontsize+0.5) * padratio_xy /1.6;
  Float_t yoffset = ((Float_t)yoffset_x/titlefontsize+0.5) * padratio_yx /1.6;
  Float_t zoffset = ((Float_t)zoffset_x/titlefontsize+0.5) * padratio_yx /1.6;
  Float_t titlesize = (Float_t)titlefontsize/padsize;
  Float_t labelsize = (Float_t)labelfontsize/padsize;
  gStyle->SetOptStat(0);
  if (std::string(histogram -> GetTitle()).size())
  {
    gStyle->SetOptTitle(1);
    gStyle->SetTitleH(titlefontsize/padsize);
    gStyle->SetTitleFontSize(titlesize*0.8);
    gStyle->SetTitleBorderSize(0);
    gStyle->SetTitleAlign(title_align);
    gStyle->SetTitleX(title_x);
    gStyle->SetTitleY(title_y);
  }
  histogram -> SetTitleFont(42,"xyz");
  histogram -> SetLabelFont(42,"xyz");
  histogram -> SetTitleSize(titlesize,"xyz");
  histogram -> SetLabelSize(labelsize,"xyz");
  histogram -> GetXaxis()->SetTitleOffset(xoffset);
  histogram -> GetYaxis()->SetTitleOffset(yoffset);
  histogram -> GetZaxis()->SetTitleOffset(zoffset);
  histogram -> GetYaxis()->SetDecimals(1);
  histogram -> GetZaxis()->SetDecimals(1);
  TVirtualPad* pad = canvas->cd(1);
  pad->SetLeftMargin((Float_t)mar_left/padsize_x);
  pad->SetRightMargin((Float_t)mar_right/padsize_x);
  pad->SetTopMargin((Float_t)mar_top/padsize_y);
  pad->SetBottomMargin((Float_t)mar_bottom/padsize_y);
  canvas->SetGrid(gx,gy);
}

void redesignCanvas(TCanvas* canvas, TH2* histogram, const std::string& draw="COLZ")
{
  int gx = 0;
  int gy = 0;
  int histosize_x = 500;
  int histosize_y = 500;
  int mar_left = 80;
  int mar_right = 120;
  int mar_top = 20;
  int mar_bottom = 60;
  int title_align = 33;
  float title_x = 1.0;
  float title_y = 1.0;
  if (std::string(histogram -> GetTitle()).size()>0&&title_y==1.0) mar_top += 20;
  int titlefontsize = 32;
  int labelfontsize = 20;
  int pal_offset_x = 5;
  int pal_width_x = 25;
  int xoffset_y = mar_bottom - titlefontsize - 4;
  int yoffset_x = mar_left - titlefontsize - 4;
  int zoffset_x = mar_right - pal_offset_x - pal_width_x - titlefontsize;
  int padsize_x = histosize_x + mar_left + mar_right;
  int padsize_y = histosize_y + mar_top + mar_bottom;
  int padsize = ((padsize_x<=padsize_y) ? padsize_x : padsize_y);
  float padratio_yx = (Float_t)padsize_y/padsize_x > 1 ? 1 : (Float_t)padsize_y/padsize_x;
  float padratio_xy = (Float_t)padsize_x/padsize_y > 1 ? 1 : (Float_t)padsize_x/padsize_y;
  Float_t xoffset = ((Float_t)xoffset_y/titlefontsize+0.5) * padratio_xy /1.6;
  Float_t yoffset = ((Float_t)yoffset_x/titlefontsize+0.5) * padratio_yx /1.6;
  Float_t zoffset = ((Float_t)zoffset_x/titlefontsize+0.5) * padratio_yx /1.6;
  Float_t titlesize = (Float_t)titlefontsize/padsize;
  Float_t labelsize = (Float_t)labelfontsize/padsize;
  histogram -> SetTitleFont(42,"xyz");
  histogram -> SetLabelFont(42,"xyz");
  histogram -> SetTitleSize(titlesize,"xyz");
  histogram -> SetLabelSize(labelsize,"xyz");
  histogram -> GetXaxis()->SetTitleOffset(xoffset);
  histogram -> GetYaxis()->SetTitleOffset(yoffset);
  histogram -> GetZaxis()->SetTitleOffset(zoffset);
  histogram -> GetZaxis()->RotateTitle(1);
  histogram -> GetYaxis()->SetDecimals(1);
  histogram -> GetZaxis()->SetDecimals(1);
  if (histosize_y<250) histogram -> GetZaxis()->SetNdivisions(505);
  if (std::string(histogram -> GetTitle()).size())
  {
    gStyle->SetOptTitle(1);
    gStyle->SetTitleH(titlefontsize/padsize);
    gStyle->SetTitleFontSize(titlesize*0.8);
    gStyle->SetTitleBorderSize(0);
    gStyle->SetTitleAlign(title_align);
    gStyle->SetTitleX(title_x);
    gStyle->SetTitleY(title_y);
  }
  TVirtualPad* pad = canvas->cd(1);
  pad->SetLeftMargin((Float_t)mar_left/padsize_x);
  pad->SetRightMargin((Float_t)mar_right/padsize_x);
  pad->SetTopMargin((Float_t)mar_top/padsize_y);
  pad->SetBottomMargin((Float_t)mar_bottom/padsize_y);
  canvas->SetGrid(gx,gy);
  histogram -> Draw(draw.c_str());
  if (histogram -> Integral()>0 && draw=="COLZ")
  {
    gPad->Update();
    TPaletteAxis* palette = (TPaletteAxis*)histogram -> GetListOfFunctions()->FindObject("palette");
    if(palette)
    {
      palette->SetX1NDC(1 - (Float_t)(mar_right - pal_offset_x)/padsize_x);
      palette->SetX2NDC(1 - (Float_t)(mar_right - pal_offset_x - pal_width_x)/padsize_x);
      palette->SetY1NDC((Float_t)mar_bottom/padsize_y);
      palette->SetY2NDC(1 - (Float_t)mar_top/padsize_y);
    }
  }
}

#endif