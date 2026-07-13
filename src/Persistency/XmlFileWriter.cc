/**
 *  @file   PandoraSDK/src/Persistency/XmlFileWriter.cc
 *
 *  @brief  Implementation of the file writer class.
 *
 *  $Log: $
 */

#include "Api/PandoraContentApi.h"
#include "Api/PandoraContentApiImpl.h"

#include "Geometry/DetectorGap.h"
#include "Geometry/LArTPC.h"
#include "Geometry/SubDetector.h"

#include "Objects/CaloHit.h"
#include "Objects/MCParticle.h"
#include "Objects/Track.h"

#include "Persistency/XmlFileWriter.h"

namespace pandora
{

XmlFileWriter::XmlFileWriter(const pandora::Pandora &pandora, const std::string &fileName, const FileMode fileMode,
    const unsigned int majorVersion, const unsigned int minorVersion) :
    FileWriter(pandora, fileName, majorVersion, minorVersion),
    m_pContainerXmlElement(nullptr),
    m_pCurrentXmlElement(nullptr)
{
    m_fileType = XML;

    if (APPEND == fileMode)
    {
        m_pXmlDocument = new TiXmlDocument(fileName);

        if (!m_pXmlDocument->LoadFile())
        {
            std::cout << "XmlFileWriter, fileMode: Append - Invalid xml file." << std::endl;
            delete m_pXmlDocument;
            throw StatusCodeException(STATUS_CODE_FAILURE);
        }
    }
    else if (OVERWRITE == fileMode)
    {
        m_pXmlDocument = new TiXmlDocument;
    }
    else
    {
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

XmlFileWriter::~XmlFileWriter()
{
    m_pXmlDocument->SaveFile(m_fileName);
    delete m_pXmlDocument;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteHeader(const ContainerId containerId)
{
    const std::string containerXmlKey((HEADER_CONTAINER == containerId) ? "Header"
            : (GEOMETRY_CONTAINER == containerId)                       ? "Geometry"
            : (EVENT_CONTAINER == containerId)                          ? "Event"
                                                                        : "Unknown");

    m_pContainerXmlElement = new TiXmlElement(containerXmlKey);
    m_pXmlDocument->LinkEndChild(m_pContainerXmlElement);

    m_containerId = containerId;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteFooter()
{
    if ((HEADER_CONTAINER != m_containerId) && (EVENT_CONTAINER != m_containerId) && (GEOMETRY_CONTAINER != m_containerId))
        return STATUS_CODE_FAILURE;

    m_containerId = UNKNOWN_CONTAINER;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteVersion()
{
    if (HEADER_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    m_pCurrentXmlElement = new TiXmlElement("Version");
    PandoraReturnOnError(this->WriteVariable("MajorVersion", m_fileMajorVersion));
    PandoraReturnOnError(this->WriteVariable("MinorVersion", m_fileMinorVersion));

    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteSubDetector(const SubDetector *const pSubDetector)
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    m_pCurrentXmlElement = new TiXmlElement("SubDetector");
    PandoraReturnOnError(m_pSubDetectorFactory->Write(pSubDetector, *this));

    PandoraReturnOnError(this->WriteVariable("SubDetectorName", pSubDetector->GetSubDetectorName()));
    PandoraReturnOnError(this->WriteVariable("SubDetectorType", pSubDetector->GetSubDetectorType()));
    PandoraReturnOnError(this->WriteVariable("InnerRCoordinate", pSubDetector->GetInnerRCoordinate()));
    PandoraReturnOnError(this->WriteVariable("InnerZCoordinate", pSubDetector->GetInnerZCoordinate()));
    PandoraReturnOnError(this->WriteVariable("InnerPhiCoordinate", pSubDetector->GetInnerPhiCoordinate()));
    PandoraReturnOnError(this->WriteVariable("InnerSymmetryOrder", pSubDetector->GetInnerSymmetryOrder()));
    PandoraReturnOnError(this->WriteVariable("OuterRCoordinate", pSubDetector->GetOuterRCoordinate()));
    PandoraReturnOnError(this->WriteVariable("OuterZCoordinate", pSubDetector->GetOuterZCoordinate()));
    PandoraReturnOnError(this->WriteVariable("OuterPhiCoordinate", pSubDetector->GetOuterPhiCoordinate()));
    PandoraReturnOnError(this->WriteVariable("OuterSymmetryOrder", pSubDetector->GetOuterSymmetryOrder()));
    PandoraReturnOnError(this->WriteVariable("IsMirroredInZ", pSubDetector->IsMirroredInZ()));

    const unsigned int nLayers(pSubDetector->GetNLayers());
    const SubDetector::SubDetectorLayerVector &subDetectorLayerVector(pSubDetector->GetSubDetectorLayerVector());

    if (subDetectorLayerVector.size() != nLayers)
        return STATUS_CODE_FAILURE;

    PandoraReturnOnError(this->WriteVariable("NLayers", nLayers));

    if (nLayers > 0)
    {
        std::string closestDistanceToIpString, nRadiationLengthsString, nInteractionLengthsString;

        for (unsigned int iLayer = 0; iLayer < nLayers; ++iLayer)
        {
            closestDistanceToIpString += TypeToString(subDetectorLayerVector.at(iLayer).GetClosestDistanceToIp()) + " ";
            nRadiationLengthsString += TypeToString(subDetectorLayerVector.at(iLayer).GetNRadiationLengths()) + " ";
            nInteractionLengthsString += TypeToString(subDetectorLayerVector.at(iLayer).GetNInteractionLengths()) + " ";
        }

        PandoraReturnOnError(this->WriteVariable("ClosestDistanceToIp", closestDistanceToIpString));
        PandoraReturnOnError(this->WriteVariable("NRadiationLengths", nRadiationLengthsString));
        PandoraReturnOnError(this->WriteVariable("NInteractionLengths", nInteractionLengthsString));
    }

    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);
    m_pCurrentXmlElement = nullptr;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteLArTPC(const LArTPC *const pLArTPC)
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    m_pCurrentXmlElement = new TiXmlElement("LArTPC");
    PandoraReturnOnError(m_pLArTPCFactory->Write(pLArTPC, *this));

    PandoraReturnOnError(this->WriteVariable("LArTPCVolumeId", pLArTPC->GetLArTPCVolumeId()));
    PandoraReturnOnError(this->WriteVariable("CenterX", pLArTPC->GetCenterX()));
    PandoraReturnOnError(this->WriteVariable("CenterY", pLArTPC->GetCenterY()));
    PandoraReturnOnError(this->WriteVariable("CenterZ", pLArTPC->GetCenterZ()));
    PandoraReturnOnError(this->WriteVariable("WidthX", pLArTPC->GetWidthX()));
    PandoraReturnOnError(this->WriteVariable("WidthY", pLArTPC->GetWidthY()));
    PandoraReturnOnError(this->WriteVariable("WidthZ", pLArTPC->GetWidthZ()));
    PandoraReturnOnError(this->WriteVariable("WirePitchU", pLArTPC->GetWirePitchU()));
    PandoraReturnOnError(this->WriteVariable("WirePitchV", pLArTPC->GetWirePitchV()));
    PandoraReturnOnError(this->WriteVariable("WirePitchW", pLArTPC->GetWirePitchW()));
    PandoraReturnOnError(this->WriteVariable("WireAngleU", pLArTPC->GetWireAngleU()));
    PandoraReturnOnError(this->WriteVariable("WireAngleV", pLArTPC->GetWireAngleV()));
    PandoraReturnOnError(this->WriteVariable("WireAngleW", pLArTPC->GetWireAngleW()));
    PandoraReturnOnError(this->WriteVariable("SigmaUVW", pLArTPC->GetSigmaUVW()));
    PandoraReturnOnError(this->WriteVariable("IsDriftInPositiveX", pLArTPC->IsDriftInPositiveX()));

    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);
    m_pCurrentXmlElement = nullptr;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteDetectorGap(const DetectorGap *const pDetectorGap)
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    const LineGap *pLineGap(nullptr);
    pLineGap = dynamic_cast<const LineGap *>(pDetectorGap);

    const BoxGap *pBoxGap(nullptr);
    pBoxGap = dynamic_cast<const BoxGap *>(pDetectorGap);

    const ConcentricGap *pConcentricGap(nullptr);
    pConcentricGap = dynamic_cast<const ConcentricGap *>(pDetectorGap);

    if (nullptr != pLineGap)
    {
        m_pCurrentXmlElement = new TiXmlElement("LineGap");
        PandoraReturnOnError(m_pLineGapFactory->Write(pLineGap, *this));

        PandoraReturnOnError(this->WriteVariable("LineGapType", pLineGap->GetLineGapType()));
        PandoraReturnOnError(this->WriteVariable("LineStartX", pLineGap->GetLineStartX()));
        PandoraReturnOnError(this->WriteVariable("LineEndX", pLineGap->GetLineEndX()));
        PandoraReturnOnError(this->WriteVariable("LineStartZ", pLineGap->GetLineStartZ()));
        PandoraReturnOnError(this->WriteVariable("LineEndZ", pLineGap->GetLineEndZ()));

        m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);
        m_pCurrentXmlElement = nullptr;
    }
    else if (nullptr != pBoxGap)
    {
        m_pCurrentXmlElement = new TiXmlElement("BoxGap");
        PandoraReturnOnError(m_pBoxGapFactory->Write(pBoxGap, *this));

        PandoraReturnOnError(this->WriteVariable("Vertex", pBoxGap->GetVertex()));
        PandoraReturnOnError(this->WriteVariable("Side1", pBoxGap->GetSide1()));
        PandoraReturnOnError(this->WriteVariable("Side2", pBoxGap->GetSide2()));
        PandoraReturnOnError(this->WriteVariable("Side3", pBoxGap->GetSide3()));

        m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);
        m_pCurrentXmlElement = nullptr;
    }
    else if (nullptr != pConcentricGap)
    {
        m_pCurrentXmlElement = new TiXmlElement("ConcentricGap");
        PandoraReturnOnError(m_pConcentricGapFactory->Write(pConcentricGap, *this));

        PandoraReturnOnError(this->WriteVariable("MinZCoordinate", pConcentricGap->GetMinZCoordinate()));
        PandoraReturnOnError(this->WriteVariable("MaxZCoordinate", pConcentricGap->GetMaxZCoordinate()));
        PandoraReturnOnError(this->WriteVariable("InnerRCoordinate", pConcentricGap->GetInnerRCoordinate()));
        PandoraReturnOnError(this->WriteVariable("InnerPhiCoordinate", pConcentricGap->GetInnerPhiCoordinate()));
        PandoraReturnOnError(this->WriteVariable("InnerSymmetryOrder", pConcentricGap->GetInnerSymmetryOrder()));
        PandoraReturnOnError(this->WriteVariable("OuterRCoordinate", pConcentricGap->GetOuterRCoordinate()));
        PandoraReturnOnError(this->WriteVariable("OuterPhiCoordinate", pConcentricGap->GetOuterPhiCoordinate()));
        PandoraReturnOnError(this->WriteVariable("OuterSymmetryOrder", pConcentricGap->GetOuterSymmetryOrder()));

        m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);
        m_pCurrentXmlElement = nullptr;
    }
    else
    {
        return STATUS_CODE_FAILURE;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteCaloHit(const CaloHit *const pCaloHit)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    m_pCurrentXmlElement = new TiXmlElement("CaloHit");
    PandoraReturnOnError(m_pCaloHitFactory->Write(pCaloHit, *this));

    const CellGeometry cellGeometry(pCaloHit->GetCellGeometry());
    PandoraReturnOnError(this->WriteVariable("CellGeometry", cellGeometry));
    PandoraReturnOnError(this->WriteVariable("PositionVector", pCaloHit->GetPositionVector()));
    PandoraReturnOnError(this->WriteVariable("ExpectedDirection", pCaloHit->GetExpectedDirection()));
    PandoraReturnOnError(this->WriteVariable("CellNormalVector", pCaloHit->GetCellNormalVector()));
    PandoraReturnOnError(this->WriteVariable("CellThickness", pCaloHit->GetCellThickness()));
    PandoraReturnOnError(this->WriteVariable("NCellRadiationLengths", pCaloHit->GetNCellRadiationLengths()));
    PandoraReturnOnError(this->WriteVariable("NCellInteractionLengths", pCaloHit->GetNCellInteractionLengths()));
    PandoraReturnOnError(this->WriteVariable("Time", pCaloHit->GetTime()));
    PandoraReturnOnError(this->WriteVariable("InputEnergy", pCaloHit->GetInputEnergy()));
    PandoraReturnOnError(this->WriteVariable("MipEquivalentEnergy", pCaloHit->GetMipEquivalentEnergy()));
    PandoraReturnOnError(this->WriteVariable("ElectromagneticEnergy", pCaloHit->GetElectromagneticEnergy()));
    PandoraReturnOnError(this->WriteVariable("HadronicEnergy", pCaloHit->GetHadronicEnergy()));
    PandoraReturnOnError(this->WriteVariable("IsDigital", pCaloHit->IsDigital()));
    PandoraReturnOnError(this->WriteVariable("HitType", pCaloHit->GetHitType()));
    PandoraReturnOnError(this->WriteVariable("HitRegion", pCaloHit->GetHitRegion()));
    PandoraReturnOnError(this->WriteVariable("Layer", pCaloHit->GetLayer()));
    PandoraReturnOnError(this->WriteVariable("IsInOuterSamplingLayer", pCaloHit->IsInOuterSamplingLayer()));
    PandoraReturnOnError(this->WriteVariable("ParentCaloHitAddress", pCaloHit->GetParentAddress()));
    PandoraReturnOnError(this->WriteVariable("CellSize0", pCaloHit->GetCellSize0()));
    PandoraReturnOnError(this->WriteVariable("CellSize1", pCaloHit->GetCellSize1()));

    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);
    m_pCurrentXmlElement = nullptr;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteTrack(const Track *const pTrack)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    m_pCurrentXmlElement = new TiXmlElement("Track");
    PandoraReturnOnError(m_pTrackFactory->Write(pTrack, *this));

    PandoraReturnOnError(this->WriteVariable("D0", pTrack->GetD0()));
    PandoraReturnOnError(this->WriteVariable("Z0", pTrack->GetZ0()));
    PandoraReturnOnError(this->WriteVariable("ParticleId", pTrack->GetParticleId()));
    PandoraReturnOnError(this->WriteVariable("Charge", pTrack->GetCharge()));
    PandoraReturnOnError(this->WriteVariable("Mass", pTrack->GetMass()));
    PandoraReturnOnError(this->WriteVariable("MomentumAtDca", pTrack->GetMomentumAtDca()));
    PandoraReturnOnError(this->WriteVariable("TrackStateAtStart", pTrack->GetTrackStateAtStart()));
    PandoraReturnOnError(this->WriteVariable("TrackStateAtEnd", pTrack->GetTrackStateAtEnd()));
    PandoraReturnOnError(this->WriteVariable("TrackStateAtCalorimeter", pTrack->GetTrackStateAtCalorimeter()));
    PandoraReturnOnError(this->WriteVariable("TimeAtCalorimeter", pTrack->GetTimeAtCalorimeter()));
    PandoraReturnOnError(this->WriteVariable("ReachesCalorimeter", pTrack->ReachesCalorimeter()));
    PandoraReturnOnError(this->WriteVariable("IsProjectedToEndCap", pTrack->IsProjectedToEndCap()));
    PandoraReturnOnError(this->WriteVariable("CanFormPfo", pTrack->CanFormPfo()));
    PandoraReturnOnError(this->WriteVariable("CanFormClusterlessPfo", pTrack->CanFormClusterlessPfo()));
    PandoraReturnOnError(this->WriteVariable("ParentTrackAddress", pTrack->GetParentAddress()));

    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);
    m_pCurrentXmlElement = nullptr;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteMCParticle(const MCParticle *const pMCParticle)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    m_pCurrentXmlElement = new TiXmlElement("MCParticle");
    PandoraReturnOnError(m_pMCParticleFactory->Write(pMCParticle, *this));

    PandoraReturnOnError(this->WriteVariable("Energy", pMCParticle->GetEnergy()));
    PandoraReturnOnError(this->WriteVariable("Momentum", pMCParticle->GetMomentum()));
    PandoraReturnOnError(this->WriteVariable("Vertex", pMCParticle->GetVertex()));
    PandoraReturnOnError(this->WriteVariable("Endpoint", pMCParticle->GetEndpoint()));
    PandoraReturnOnError(this->WriteVariable("ParticleId", pMCParticle->GetParticleId()));
    PandoraReturnOnError(this->WriteVariable("MCParticleType", pMCParticle->GetMCParticleType()));
    PandoraReturnOnError(this->WriteVariable("Uid", pMCParticle->GetUid()));

    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);
    m_pCurrentXmlElement = nullptr;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteRelationship(const RelationshipId relationshipId, const void *address1, const void *address2, const float weight)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    m_pCurrentXmlElement = new TiXmlElement("Relationship");

    PandoraReturnOnError(this->WriteVariable("RelationshipId", relationshipId));
    PandoraReturnOnError(this->WriteVariable("Address1", address1));
    PandoraReturnOnError(this->WriteVariable("Address2", address2));
    PandoraReturnOnError(this->WriteVariable("Weight", weight));

    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);
    m_pCurrentXmlElement = nullptr;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteEventInformation()
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    m_pCurrentXmlElement = new TiXmlElement("EventInfo");

    PandoraReturnOnError(this->WriteVariable("Run", m_pPandora->GetRun()));
    PandoraReturnOnError(this->WriteVariable("Subrun", m_pPandora->GetSubrun()));
    PandoraReturnOnError(this->WriteVariable("Event", m_pPandora->GetEvent()));

    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);
    m_pCurrentXmlElement = nullptr;

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
