#include <Convolution.h>
#include <IntegrablePdf.h>
#include <iostream>
#include <PdfExceptions.h>
#include <SystematicExceptions.h>

// Catch the pdf errors from parameter setting and rethrow as systematic exceptions, so they
// can be treated generically with the other systematics
void Convolution::SetPdf(IntegrablePdf* pdf_){
    fPdf = dynamic_cast<IntegrablePdf*>(pdf_->Clone());
    fParameterCount = fPdf->GetParameters().size();
    if(!fPdf)
        throw InitialisationError("Non-Integrable pdf used for convolution!");
}

Convolution::~Convolution(){
    delete fPdf;
}

void Convolution::SetAxes(const AxisCollection& axes_){
    fPdfMapping.SetAxes(axes_);
    fHasAxes = true;
}

void 
Convolution::Construct(){
    if (!fPdf || !fHasAxes)
        throw InitialisationError("Tried to construct convolution without axes and pdf!");
    
    if(!fCachedCompatibleBins)
        CacheCompatibleBins();

    size_t nBins = fPdfMapping.GetNBins();
    size_t nDims = fPdfMapping.GetAxes().GetNDimensions();
    const AxisCollection& axes = fPdfMapping.GetAxes();
    std::vector<size_t> relativeIndices = fDataRep.GetRelativeIndices(fPdfDataRep);

    // Work out the transition probabilitites within this sub set of the bins
    std::vector<double> binCentres(fSysAxes.GetNDimensions());
    std::vector<double> lowEdges(fSysAxes.GetNDimensions());
    std::vector<double> highEdges(fSysAxes.GetNDimensions());

    PdfMapping subMap;
    subMap.SetAxes(fSysAxes);

    for (size_t origBin = 0; origBin < fSysAxes.GetNBins(); origBin++){
        // get the centre of the bin. Need to offset by this for a convolution
        fSysAxes.GetBinCentres(origBin, binCentres);

        // loop over the bins it can be smeared into 
        for(size_t destBin = 0; destBin < fSysAxes.GetNBins(); destBin++){
            fSysAxes.GetBinLowEdges(destBin, lowEdges);
            fSysAxes.GetBinHighEdges(destBin, highEdges);
            
            for(size_t i = 0; i < fSysAxes.GetNDimensions(); i++){
                lowEdges[i] -= binCentres.at(i);
                highEdges[i] -= binCentres.at(i);
            }
	    subMap.SetComponent(destBin, origBin, fPdf -> Integral(lowEdges, highEdges));
        }        
    }

    // Now expand to the full size matrix. Elements are zero by default
    // compatible bins are cached, values must match the smaller matrix above
    size_t destBin = -1;
    std::vector<unsigned> nonZeroRowIndices;
    std::vector<unsigned> nonZeroColIndices;
    std::vector<double> values;
    nonZeroRowIndices.reserve(fCompatibleBins.at(0).size());
    nonZeroColIndices.reserve(fCompatibleBins.at(0).size());
    

    for(size_t origBin = 0; origBin < axes.GetNBins(); origBin++){
        for(size_t i = 0; i < fCompatibleBins.at(origBin).size(); i++){
            destBin = fCompatibleBins.at(origBin).at(i);
            nonZeroRowIndices.push_back(origBin);
            nonZeroColIndices.push_back(destBin);
            values.push_back( subMap.GetComponent(fSysBins.at(origBin),
                                                  fSysBins.at(destBin)));
        }
    }
        
    fPdfMapping.SetComponents(nonZeroRowIndices, nonZeroColIndices, values);
}

void
Convolution::SetParameters(const std::vector<double>& params_){
    try{
    fPdf->SetParameters(params_);
    }
    catch (const ParameterError& e_){
        throw InvalidSystematicParameter(std::string("Convolution: couldnt set pdf params, invalid val:  ") + e_.what());
    }
    catch (const DimensionError& e_){
        throw WrongNumberOfParameters(std::string("Convolution: tried to change pdf params with wrong number") + e_.what());
    }

}

std::vector<double>
Convolution::GetParameters() const{
    return fPdf->GetParameters();
}

double 
Convolution::GetParameter(size_t index_) const{
    try{
        return fPdf->GetParameter(index_);
    }
    catch(const DimensionError& e_){
        throw WrongNumberOfParameters("Convolution: Tried to access a variable the pdf does not have!");
    }
}

void
Convolution::SetParameter(size_t index_, double val_){
    try{
        fPdf->SetParameter(index_, val_);
    }
    catch(const DimensionError& e_){
        throw WrongNumberOfParameters("Convolution: Tried to access a variable the pdf does not have!");       
    }
}

void
Convolution::CacheCompatibleBins(){
    fCompatibleBins.resize(fPdfMapping.GetNBins());
    // only need to look at one side of the matrix, its symmetric
    for(size_t i = 0; i < fPdfMapping.GetNBins(); i++){
        for(size_t j = i+1;  j < fPdfMapping.GetNBins(); j++){
            if(BinsCompatible(i , j)){
                fCompatibleBins.at(i).push_back(j);
                fCompatibleBins.at(j).push_back(i);
            }
        }
    }

    std::vector<size_t> relativeIndices = fDataRep.GetRelativeIndices(fPdfDataRep);
    const AxisCollection& axes = fPdfMapping.GetAxes();

    //  get the axes that this systematic will act on
    fSysAxes = AxisCollection();
    for(size_t i = 0; i < relativeIndices.size(); i++)
      fSysAxes.AddAxis(axes.GetAxis(relativeIndices.at(i)));
    
    // cache the equivilent index in the binning system of the systematic
    fSysBins.resize(fPdfMapping.GetNBins());
    std::vector<size_t> sysIndices(relativeIndices.size(), 0);
    for(size_t i = 0; i < axes.GetNBins(); i++){
      for(size_t dim = 0; dim < relativeIndices.size(); dim++)
	sysIndices[dim] = axes.UnflattenIndex(i, relativeIndices.at(dim));

      fSysBins[i] = fSysAxes.FlattenIndices(sysIndices);
    }
    fCachedCompatibleBins = true;
}
