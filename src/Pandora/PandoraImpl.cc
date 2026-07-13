/**
 *  @file   PandoraSDK/src/Pandora/PandoraImpl.cc
 * 
 *  @brief  Implementation of the pandora impl class.
 * 
 *  $Log: $
 */

#include "Api/PandoraContentApiImpl.h"

#include "Managers/AlgorithmManager.h"
#include "Managers/CaloHitManager.h"
#include "Managers/ClusterManager.h"
#include "Managers/MCManager.h"
#include "Managers/ParticleFlowObjectManager.h"
#include "Managers/PluginManager.h"
#include "Managers/TrackManager.h"
#include "Managers/VertexManager.h"

#include "Objects/EventContext.h"

#include "Pandora/Pandora.h"
#include "Pandora/PandoraImpl.h"
#include "Pandora/PandoraSettings.h"

namespace pandora
{

StatusCode PandoraImpl::PrepareMCParticles() const
{
    PandoraReturnOnError(m_pPandora->m_pMCManager->CreateInputList());
    PandoraReturnOnError(m_pPandora->m_pMCManager->AddMCParticleRelationships());
    PandoraReturnOnError(m_pPandora->m_pMCManager->IdentifyPfoTargets());

    UidToMCParticleWeightMap caloHitToPfoTargetsMap;
    PandoraReturnOnError(m_pPandora->m_pMCManager->CreateCaloHitToPfoTargetsMap(caloHitToPfoTargetsMap));
    PandoraReturnOnError(m_pPandora->m_pCaloHitManager->MatchCaloHitsToMCPfoTargets(caloHitToPfoTargetsMap));

    UidToMCParticleWeightMap trackToPfoTargetsMap;
    PandoraReturnOnError(m_pPandora->m_pMCManager->CreateTrackToPfoTargetsMap(trackToPfoTargetsMap));
    PandoraReturnOnError(m_pPandora->m_pTrackManager->MatchTracksToMCPfoTargets(trackToPfoTargetsMap));

    PandoraReturnOnError(m_pPandora->m_pMCManager->SelectPfoTargets());

    return STATUS_CODE_SUCCESS;
}
//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraImpl::PrepareTracks() const
{
    PandoraReturnOnError(m_pPandora->m_pTrackManager->CreateInputList());
    PandoraReturnOnError(m_pPandora->m_pTrackManager->AssociateTracks());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraImpl::PrepareCaloHits() const
{
    return m_pPandora->m_pCaloHitManager->CreateInputList();
}

//------------------------------------------------------------------------------------------------------------------------------------------

const StringVector &PandoraImpl::GetPandoraAlgorithms() const
{
    return m_pPandora->m_pAlgorithmManager->GetPandoraAlgorithms();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraImpl::RunAlgorithm(const std::string &algorithmName) const
{
    return m_pPandora->m_pPandoraContentApiImpl->RunAlgorithm(algorithmName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraImpl::InitializeSettings(const TiXmlHandle *const pXmlHandle) const
{
    return m_pPandora->m_pPandoraSettings->Initialize(pXmlHandle);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraImpl::InitializeAlgorithms(const TiXmlHandle *const pXmlHandle) const
{
    return m_pPandora->m_pAlgorithmManager->InitializeAlgorithms(pXmlHandle);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraImpl::InitializePlugins(const TiXmlHandle *const pXmlHandle) const
{
    return m_pPandora->m_pPluginManager->InitializePlugins(pXmlHandle);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraImpl::ResetEvent() const
{
    PandoraReturnOnError(m_pPandora->m_pCaloHitManager->ResetForNextEvent());
    PandoraReturnOnError(m_pPandora->m_pClusterManager->ResetForNextEvent());
    PandoraReturnOnError(m_pPandora->m_pMCManager->ResetForNextEvent());
    PandoraReturnOnError(m_pPandora->m_pPfoManager->ResetForNextEvent());
    PandoraReturnOnError(m_pPandora->m_pTrackManager->ResetForNextEvent());
    PandoraReturnOnError(m_pPandora->m_pVertexManager->ResetForNextEvent());
    PandoraReturnOnError(m_pPandora->m_pAlgorithmManager->ResetForNextEvent());
    PandoraReturnOnError(m_pPandora->m_pPluginManager->ResetForNextEvent());
    PandoraReturnOnError(m_pPandora->m_pEventContext->ResetForNextEvent());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

PandoraImpl::PandoraImpl(Pandora *const pPandora) :
    m_pPandora(pPandora)
{
}

} // namespace pandora
