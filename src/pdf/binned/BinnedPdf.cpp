#include <BinnedPdf.h>
#include <PdfExceptions.h>
#include <DataExceptions.h>

BinnedPdf::BinnedPdf(const AxisCollection& axes_){
    fHistogram.SetAxes(axes_);
    fNDims = axes_.GetNDimensions();
}

BinnedPdf::BinnedPdf(const Histogram& histo_){
    fHistogram = histo_;
    fNDims     = histo_.GetNDims();
}

BinnedPdf::BinnedPdf(const BinnedPdf& other_) : Pdf(other_){
    fHistogram.SetAxes(other_.GetAxes());
    fHistogram.SetBinContents(other_.GetBinContents());
    fDataRep = other_.fDataRep;
    fNDims = other_.fNDims;
}

BinnedPdf
BinnedPdf::operator=(const BinnedPdf& other_){
    fHistogram.SetAxes(other_.GetAxes());
    fHistogram.SetBinContents(other_.GetBinContents());
    fDataRep = other_.fDataRep;
    fNDims = other_.fNDims;
    return *this;
}

const Histogram&
BinnedPdf::GetHistogram() const{
    return fHistogram;
}

void
BinnedPdf::SetHistogram(const Histogram& hist_){
    fHistogram = hist_;
    fNDims = hist_.GetNDims();
}

void 
BinnedPdf::Fill(const EventData& data_, double weight_){
    try{
        fHistogram.Fill(data_.ToRepresentation(fDataRep), weight_);
    }
    catch (const DimensionError& e_){
        throw RepresentationError(std::string("Representation in compatible with pdf ") + e_.what());
    }
}

size_t 
BinnedPdf::FindBin(const EventData& data_) const{
    try{
        return fHistogram.FindBin(data_.ToRepresentation(fDataRep));    
    }
    catch (const DimensionError& e_){
        throw RepresentationError(std::string("Representation in compatible with pdf ") + e_.what());
    }    
}

Pdf* 
BinnedPdf::Clone() const{
    return static_cast<Pdf*>(new BinnedPdf(*this));
}

//////////////////////////////////////////////////////////////////////////////////////////
// All methods below this line just forward the call to the underlying histogram object //
//////////////////////////////////////////////////////////////////////////////////////////

void 
BinnedPdf::SetAxes(const AxisCollection& axes_){
    fHistogram.SetAxes(axes_);

}

const AxisCollection& 
BinnedPdf::GetAxes() const{
    return fHistogram.GetAxes();
}

double 
BinnedPdf::operator() (const std::vector<double>& vals_) const{
    return fHistogram.operator()(vals_);
}

double 
BinnedPdf::Integral() const{
    return fHistogram.Integral();
}

void 
BinnedPdf::Normalise(){
    fHistogram.Normalise();
}


void 
BinnedPdf::Fill(const std::vector<double>& vals_, double weight_){
    fHistogram.Fill(vals_, weight_);
}


void 
BinnedPdf::Fill(double vals_, double weight_){
    fHistogram.Fill(vals_, weight_);
}

size_t 
BinnedPdf::FindBin(const std::vector<double>& vals_) const{
    return fHistogram.FindBin(vals_);
    
}

double 
BinnedPdf::GetBinContent(size_t bin_) const{
    return fHistogram.GetBinContent(bin_);
}

void 
BinnedPdf::AddBinContent(size_t bin_, double content_){
    fHistogram.AddBinContent(bin_, content_);
}

void 
BinnedPdf::SetBinContent(size_t bin_, double content_){
    fHistogram.SetBinContent(bin_, content_);
}

size_t 
BinnedPdf::GetNBins() const{
    return fHistogram.GetNBins();
}

void 
BinnedPdf::Empty(){
    fHistogram.Empty();
}

size_t 
BinnedPdf::FlattenIndices(const std::vector<size_t>& indices_) const{
    return fHistogram.FlattenIndices(indices_);
}

std::vector<size_t> 
BinnedPdf::UnpackIndices(size_t bin_) const{
    return fHistogram.UnpackIndices(bin_);
}

std::vector<double> 
BinnedPdf::GetBinContents() const{
    return fHistogram.GetBinContents();
}
void 
BinnedPdf::SetBinContents(const std::vector<double>& data_){
    return fHistogram.SetBinContents(data_);
}

std::vector<double>
BinnedPdf::Means() const{
    return fHistogram.Means();
}

std::vector<double>
BinnedPdf::Variances() const{
    return fHistogram.Variances();
}

BinnedPdf 
BinnedPdf::Marginalise(const std::vector<size_t>& indices_) const{
    // Find the relative indicies indicies in 
    DataRepresentation newRep = DataRepresentation(indices_);
    std::vector<size_t> relativeIndices = newRep.GetRelativeIndices(fDataRep);

    // Marginalise the histogram
	BinnedPdf newPdf(fHistogram.Marginalise(relativeIndices));
	newPdf.SetDataRep(newRep);
	return newPdf;
}

BinnedPdf
BinnedPdf::Marginalise(size_t index_) const{
    return Marginalise(std::vector<size_t>(1, index_));
}
