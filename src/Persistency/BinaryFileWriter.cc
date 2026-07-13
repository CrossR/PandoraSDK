/**
 *  @file   PandoraSDK/src/Persistency/BinaryFileWriter.cc
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

#include "Persistency/BinaryFileWriter.h"

namespace pandora
{

BinaryFileWriter::BinaryFileWriter(const pandora::Pandora &pandora, const std::string &fileName, const FileMode fileMode,
    const unsigned int majorVersion, const unsigned int minorVersion) :
    FileWriter(pandora, fileName, majorVersion, minorVersion)
{
    m_fileType = BINARY;

    if (APPEND == fileMode)
    {
        m_fileStream.open(fileName.c_str(), std::ios::out | std::ios::in | std::ios::binary | std::ios::ate);

        if (!m_fileStream.is_open())
        {
            m_fileStream.open(fileName.c_str(), std::ios::out | std::ios::binary);
        }
    }
    else if (OVERWRITE == fileMode)
    {
        m_fileStream.open(fileName.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
    }
    else
    {
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }

    if (!m_fileStream.is_open() || !m_fileStream.good())
        throw StatusCodeException(STATUS_CODE_FAILURE);

    m_containerPosition = m_fileStream.tellp();
}

//------------------------------------------------------------------------------------------------------------------------------------------

BinaryFileWriter::~BinaryFileWriter()
{
    m_fileStream.close();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteHeader(const ContainerId containerId)
{
    PandoraReturnOnError(this->WriteVariable(PANDORA_FILE_HASH));
    PandoraReturnOnError(this->WriteVariable(containerId));

    m_containerPosition = m_fileStream.tellp();
    const std::ofstream::pos_type dummyContainerSize(0);
    PandoraReturnOnError(this->WriteVariable(dummyContainerSize));

    m_containerId = containerId;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteFooter()
{
    if ((HEADER_CONTAINER != m_containerId) && (EVENT_CONTAINER != m_containerId) && (GEOMETRY_CONTAINER != m_containerId))
        return STATUS_CODE_FAILURE;

    PandoraReturnOnError(this->WriteVariable((HEADER_CONTAINER == m_containerId) ? HEADER_END_COMPONENT
                : (EVENT_CONTAINER == m_containerId)            ? EVENT_END_COMPONENT
                                                                : GEOMETRY_END_COMPONENT));

    m_containerId = UNKNOWN_CONTAINER;

    const std::ofstream::pos_type containerSize(m_fileStream.tellp() - m_containerPosition);
    m_fileStream.seekp(m_containerPosition, std::ios::beg);

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    PandoraReturnOnError(this->WriteVariable(containerSize));
    m_fileStream.seekp(0, std::ios::end);

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    m_containerPosition = m_fileStream.tellp();

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteVersion()
{
    if (HEADER_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PandoraReturnOnError(this->WriteVariable(VERSION_COMPONENT));
    PandoraReturnOnError(this->WriteVariable(m_fileMajorVersion));
    PandoraReturnOnError(this->WriteVariable(m_fileMinorVersion));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteSubDetector(const SubDetector *const pSubDetector)
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PandoraReturnOnError(this->WriteVariable(SUB_DETECTOR_COMPONENT));
    PandoraReturnOnError(m_pSubDetectorFactory->Write(pSubDetector, *this));

    PandoraReturnOnError(this->WriteVariable(pSubDetector->GetSubDetectorName()));
    PandoraReturnOnError(this->WriteVariable(pSubDetector->GetSubDetectorType()));
    PandoraReturnOnError(this->WriteVariable(pSubDetector->GetInnerRCoordinate()));
    PandoraReturnOnError(this->WriteVariable(pSubDetector->GetInnerZCoordinate()));
    PandoraReturnOnError(this->WriteVariable(pSubDetector->GetInnerPhiCoordinate()));
    PandoraReturnOnError(this->WriteVariable(pSubDetector->GetInnerSymmetryOrder()));
    PandoraReturnOnError(this->WriteVariable(pSubDetector->GetOuterRCoordinate()));
    PandoraReturnOnError(this->WriteVariable(pSubDetector->GetOuterZCoordinate()));
    PandoraReturnOnError(this->WriteVariable(pSubDetector->GetOuterPhiCoordinate()));
    PandoraReturnOnError(this->WriteVariable(pSubDetector->GetOuterSymmetryOrder()));
    PandoraReturnOnError(this->WriteVariable(pSubDetector->IsMirroredInZ()));

    const unsigned int nLayers(pSubDetector->GetNLayers());
    const SubDetector::SubDetectorLayerVector &subDetectorLayerVector(pSubDetector->GetSubDetectorLayerVector());

    if (subDetectorLayerVector.size() != nLayers)
        return STATUS_CODE_FAILURE;

    PandoraReturnOnError(this->WriteVariable(nLayers));

    for (unsigned int iLayer = 0; iLayer < nLayers; ++iLayer)
    {
        PandoraReturnOnError(this->WriteVariable(subDetectorLayerVector.at(iLayer).GetClosestDistanceToIp()));
        PandoraReturnOnError(this->WriteVariable(subDetectorLayerVector.at(iLayer).GetNRadiationLengths()));
        PandoraReturnOnError(this->WriteVariable(subDetectorLayerVector.at(iLayer).GetNInteractionLengths()));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteLArTPC(const LArTPC *const pLArTPC)
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PandoraReturnOnError(this->WriteVariable(LAR_TPC_COMPONENT));
    PandoraReturnOnError(m_pLArTPCFactory->Write(pLArTPC, *this));

    PandoraReturnOnError(this->WriteVariable(pLArTPC->GetLArTPCVolumeId()));
    PandoraReturnOnError(this->WriteVariable(pLArTPC->GetCenterX()));
    PandoraReturnOnError(this->WriteVariable(pLArTPC->GetCenterY()));
    PandoraReturnOnError(this->WriteVariable(pLArTPC->GetCenterZ()));
    PandoraReturnOnError(this->WriteVariable(pLArTPC->GetWidthX()));
    PandoraReturnOnError(this->WriteVariable(pLArTPC->GetWidthY()));
    PandoraReturnOnError(this->WriteVariable(pLArTPC->GetWidthZ()));
    PandoraReturnOnError(this->WriteVariable(pLArTPC->GetWirePitchU()));
    PandoraReturnOnError(this->WriteVariable(pLArTPC->GetWirePitchV()));
    PandoraReturnOnError(this->WriteVariable(pLArTPC->GetWirePitchW()));
    PandoraReturnOnError(this->WriteVariable(pLArTPC->GetWireAngleU()));
    PandoraReturnOnError(this->WriteVariable(pLArTPC->GetWireAngleV()));
    PandoraReturnOnError(this->WriteVariable(pLArTPC->GetWireAngleW()));
    PandoraReturnOnError(this->WriteVariable(pLArTPC->GetSigmaUVW()));
    PandoraReturnOnError(this->WriteVariable(pLArTPC->IsDriftInPositiveX()));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteDetectorGap(const DetectorGap *const pDetectorGap)
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
        PandoraReturnOnError(this->WriteVariable(LINE_GAP_COMPONENT));
        PandoraReturnOnError(m_pLineGapFactory->Write(pLineGap, *this));

        PandoraReturnOnError(this->WriteVariable(pLineGap->GetLineGapType()));
        PandoraReturnOnError(this->WriteVariable(pLineGap->GetLineStartX()));
        PandoraReturnOnError(this->WriteVariable(pLineGap->GetLineEndX()));
        PandoraReturnOnError(this->WriteVariable(pLineGap->GetLineStartZ()));
        PandoraReturnOnError(this->WriteVariable(pLineGap->GetLineEndZ()));
    }
    else if (nullptr != pBoxGap)
    {
        PandoraReturnOnError(this->WriteVariable(BOX_GAP_COMPONENT));
        PandoraReturnOnError(m_pBoxGapFactory->Write(pBoxGap, *this));

        PandoraReturnOnError(this->WriteVariable(pBoxGap->GetVertex()));
        PandoraReturnOnError(this->WriteVariable(pBoxGap->GetSide1()));
        PandoraReturnOnError(this->WriteVariable(pBoxGap->GetSide2()));
        PandoraReturnOnError(this->WriteVariable(pBoxGap->GetSide3()));
    }
    else if (nullptr != pConcentricGap)
    {
        PandoraReturnOnError(this->WriteVariable(CONCENTRIC_GAP_COMPONENT));
        PandoraReturnOnError(m_pConcentricGapFactory->Write(pConcentricGap, *this));

        PandoraReturnOnError(this->WriteVariable(pConcentricGap->GetMinZCoordinate()));
        PandoraReturnOnError(this->WriteVariable(pConcentricGap->GetMaxZCoordinate()));
        PandoraReturnOnError(this->WriteVariable(pConcentricGap->GetInnerRCoordinate()));
        PandoraReturnOnError(this->WriteVariable(pConcentricGap->GetInnerPhiCoordinate()));
        PandoraReturnOnError(this->WriteVariable(pConcentricGap->GetInnerSymmetryOrder()));
        PandoraReturnOnError(this->WriteVariable(pConcentricGap->GetOuterRCoordinate()));
        PandoraReturnOnError(this->WriteVariable(pConcentricGap->GetOuterPhiCoordinate()));
        PandoraReturnOnError(this->WriteVariable(pConcentricGap->GetOuterSymmetryOrder()));
    }
    else
    {
        return STATUS_CODE_FAILURE;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteCaloHit(const CaloHit *const pCaloHit)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PandoraReturnOnError(this->WriteVariable(CALO_HIT_COMPONENT));
    PandoraReturnOnError(m_pCaloHitFactory->Write(pCaloHit, *this));

    const CellGeometry cellGeometry(pCaloHit->GetCellGeometry());
    PandoraReturnOnError(this->WriteVariable(cellGeometry));
    PandoraReturnOnError(this->WriteVariable(pCaloHit->GetPositionVector()));
    PandoraReturnOnError(this->WriteVariable(pCaloHit->GetExpectedDirection()));
    PandoraReturnOnError(this->WriteVariable(pCaloHit->GetCellNormalVector()));
    PandoraReturnOnError(this->WriteVariable(pCaloHit->GetCellThickness()));
    PandoraReturnOnError(this->WriteVariable(pCaloHit->GetNCellRadiationLengths()));
    PandoraReturnOnError(this->WriteVariable(pCaloHit->GetNCellInteractionLengths()));
    PandoraReturnOnError(this->WriteVariable(pCaloHit->GetTime()));
    PandoraReturnOnError(this->WriteVariable(pCaloHit->GetInputEnergy()));
    PandoraReturnOnError(this->WriteVariable(pCaloHit->GetMipEquivalentEnergy()));
    PandoraReturnOnError(this->WriteVariable(pCaloHit->GetElectromagneticEnergy()));
    PandoraReturnOnError(this->WriteVariable(pCaloHit->GetHadronicEnergy()));
    PandoraReturnOnError(this->WriteVariable(pCaloHit->IsDigital()));
    PandoraReturnOnError(this->WriteVariable(pCaloHit->GetHitType()));
    PandoraReturnOnError(this->WriteVariable(pCaloHit->GetHitRegion()));
    PandoraReturnOnError(this->WriteVariable(pCaloHit->GetLayer()));
    PandoraReturnOnError(this->WriteVariable(pCaloHit->IsInOuterSamplingLayer()));
    PandoraReturnOnError(this->WriteVariable(pCaloHit->GetParentAddress()));
    PandoraReturnOnError(this->WriteVariable(pCaloHit->GetCellSize0()));
    PandoraReturnOnError(this->WriteVariable(pCaloHit->GetCellSize1()));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteTrack(const Track *const pTrack)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PandoraReturnOnError(this->WriteVariable(TRACK_COMPONENT));
    PandoraReturnOnError(m_pTrackFactory->Write(pTrack, *this));

    PandoraReturnOnError(this->WriteVariable(pTrack->GetD0()));
    PandoraReturnOnError(this->WriteVariable(pTrack->GetZ0()));
    PandoraReturnOnError(this->WriteVariable(pTrack->GetParticleId()));
    PandoraReturnOnError(this->WriteVariable(pTrack->GetCharge()));
    PandoraReturnOnError(this->WriteVariable(pTrack->GetMass()));
    PandoraReturnOnError(this->WriteVariable(pTrack->GetMomentumAtDca()));
    PandoraReturnOnError(this->WriteVariable(pTrack->GetTrackStateAtStart()));
    PandoraReturnOnError(this->WriteVariable(pTrack->GetTrackStateAtEnd()));
    PandoraReturnOnError(this->WriteVariable(pTrack->GetTrackStateAtCalorimeter()));
    PandoraReturnOnError(this->WriteVariable(pTrack->GetTimeAtCalorimeter()));
    PandoraReturnOnError(this->WriteVariable(pTrack->ReachesCalorimeter()));
    PandoraReturnOnError(this->WriteVariable(pTrack->IsProjectedToEndCap()));
    PandoraReturnOnError(this->WriteVariable(pTrack->CanFormPfo()));
    PandoraReturnOnError(this->WriteVariable(pTrack->CanFormClusterlessPfo()));
    PandoraReturnOnError(this->WriteVariable(pTrack->GetParentAddress()));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteMCParticle(const MCParticle *const pMCParticle)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PandoraReturnOnError(this->WriteVariable(MC_PARTICLE_COMPONENT));
    PandoraReturnOnError(m_pMCParticleFactory->Write(pMCParticle, *this));

    PandoraReturnOnError(this->WriteVariable(pMCParticle->GetEnergy()));
    PandoraReturnOnError(this->WriteVariable(pMCParticle->GetMomentum()));
    PandoraReturnOnError(this->WriteVariable(pMCParticle->GetVertex()));
    PandoraReturnOnError(this->WriteVariable(pMCParticle->GetEndpoint()));
    PandoraReturnOnError(this->WriteVariable(pMCParticle->GetParticleId()));
    PandoraReturnOnError(this->WriteVariable(pMCParticle->GetMCParticleType()));
    PandoraReturnOnError(this->WriteVariable(pMCParticle->GetUid()));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteRelationship(const RelationshipId relationshipId, const void *address1, const void *address2, const float weight)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PandoraReturnOnError(this->WriteVariable(RELATIONSHIP_COMPONENT));
    PandoraReturnOnError(this->WriteVariable(relationshipId));
    PandoraReturnOnError(this->WriteVariable(address1));
    PandoraReturnOnError(this->WriteVariable(address2));
    PandoraReturnOnError(this->WriteVariable(weight));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteEventInformation()
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PandoraReturnOnError(this->WriteVariable(EVENT_INFO_COMPONENT));
    PandoraReturnOnError(this->WriteVariable(m_pPandora->GetRun()));
    PandoraReturnOnError(this->WriteVariable(m_pPandora->GetSubrun()));
    PandoraReturnOnError(this->WriteVariable(m_pPandora->GetEvent()));

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
