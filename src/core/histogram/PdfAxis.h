/*****************************************************************************************************/
/* This class represents the bin boundaries in one of several observables                            */
/*                                                                                                   */
/* If constructed with a min a max and a number of bins the bins are constructed automatically with  */
/* equal bin width                                                                                   */
/*                                                                                                   */
/* The first/last bins are under/overflows                                                           */
/*****************************************************************************************************/
#ifndef __OXSX_AXIS__
#define __OXSX_AXIS__
#include <vector>
#include <string>

class PdfAxis{
 public:
    // Equal bin widths
    PdfAxis(const std::string& name_, double min_, double max_, size_t nBins_,
            const std::string& latexName_ = "");


    // Variable width
    PdfAxis(const std::string& name_, const std::vector<double>& lowEdges_, 
            const std::vector<double>& highEdges_, const std::string& latexName_ = "");

    // Copy Constructors
    PdfAxis(const PdfAxis&);
    PdfAxis operator=(const PdfAxis&);

    ~PdfAxis() {}

    // Find the Bin value_ is in
    size_t FindBin(double value_) const;

    double GetMin() const {return fMin;}
    double GetMax() const {return fMax;}
    size_t GetNBins() const {return fNBins;}
    double GetBinLowEdge (size_t i) const {return fBinLowEdges.at(i);}
    double GetBinHighEdge(size_t i) const {return fBinHighEdges.at(i);}
    double GetBinCentre  (size_t i) const {return fBinCentres.at(i);}    
	double GetBinWidth(size_t i) const {return fBinWidths.at(i);}

	std::vector<double> GetBinLowEdges() const {return fBinLowEdges;}
	std::vector<double> GetBinHighEdges() const {return fBinHighEdges;}
	std::vector<double> GetBinCentres() const {return fBinCentres;}

    std::string GetName() const {return fName;}
    std::string GetLatexName() const {return fLatexName;}

	double GetMaximum() const {return fMax;}
	double GetMinimum() const {return fMin;}


 private:
    int fNBins;
    double fMin;
    double fMax;

    std::string fName;
    std::string fLatexName; 
    
    std::vector<double> fBinLowEdges;
    std::vector<double> fBinHighEdges;
    std::vector<double> fBinCentres;
	std::vector<double> fBinWidths;
};

#endif
