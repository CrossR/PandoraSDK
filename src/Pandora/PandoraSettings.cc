/**
 *  @file PandoraSDK/src/Pandora/PandoraSettings.cc
 * 
 *  @brief Implementation of the pandora settings class.
 * 
 *  $Log: $
 */

#include "Helpers/XmlHelper.h"

#include "Pandora/PandoraSettings.h"

namespace pandora
{

PandoraSettings::PandoraSettings(const Pandora *const pPandora) :
    m_isMonitoringEnabled(false),
    m_shouldDisplayAlgorithmInfo(false),
    m_singleHitTypeClusteringMode(false),
    m_shouldCollapseMCParticlesToPfoTarget(false),
    m_useSingleMCParticleAssociation(false),
    m_electromagneticEnergyResolution(0.2f),
    m_hadronicEnergyResolution(0.6f),
    m_mcPfoSelectionRadius(500.f),
    m_mcPfoSelectionMomentum(0.01f),
    m_mcPfoSelectionLowEnergyNPCutOff(1.2f),
    m_gapTolerance(0.f),
    m_pPandora(pPandora)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

PandoraSettings::~PandoraSettings()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraSettings::Initialize(const TiXmlHandle *const pXmlHandle)
{
    try
    {
        PandoraThrowOnError(PandoraSettings::ReadGlobalSettings(pXmlHandle));

        return STATUS_CODE_SUCCESS;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to initialize pandora settings: " << statusCodeException.ToString() << std::endl;
        return statusCodeException.GetStatusCode();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraSettings::ReadGlobalSettings(const TiXmlHandle *const pXmlHandle)
{
    m_isMonitoringEnabled = false;
    PandoraReturnOnError(XmlHelper::ReadOptional(*pXmlHandle, "IsMonitoringEnabled", m_isMonitoringEnabled));

    m_shouldDisplayAlgorithmInfo = false;
    PandoraReturnOnError(XmlHelper::ReadOptional(*pXmlHandle, "ShouldDisplayAlgorithmInfo", m_shouldDisplayAlgorithmInfo));

    m_singleHitTypeClusteringMode = false;
    PandoraReturnOnError(XmlHelper::ReadOptional(*pXmlHandle, "SingleHitTypeClusteringMode", m_singleHitTypeClusteringMode));

    m_shouldCollapseMCParticlesToPfoTarget = false;
    PandoraReturnOnError(XmlHelper::ReadOptional(*pXmlHandle, "ShouldCollapseMCParticlesToPfoTarget", m_shouldCollapseMCParticlesToPfoTarget));

    m_useSingleMCParticleAssociation = false;
    PandoraReturnOnError(XmlHelper::ReadOptional(*pXmlHandle, "UseSingleMCParticleAssociation", m_useSingleMCParticleAssociation));

    m_electromagneticEnergyResolution = 0.2f;
    PandoraReturnOnError(XmlHelper::ReadOptional(*pXmlHandle, "ElectromagneticEnergyResolution", m_electromagneticEnergyResolution));

    if (m_electromagneticEnergyResolution < std::numeric_limits<float>::epsilon())
        return STATUS_CODE_INVALID_PARAMETER;

    m_hadronicEnergyResolution = 0.6f;
    PandoraReturnOnError(XmlHelper::ReadOptional(*pXmlHandle, "HadronicEnergyResolution", m_hadronicEnergyResolution));

    if (m_hadronicEnergyResolution < std::numeric_limits<float>::epsilon())
        return STATUS_CODE_INVALID_PARAMETER;

    m_mcPfoSelectionRadius = 500.f;
    PandoraReturnOnError(XmlHelper::ReadOptional(*pXmlHandle, "MCPfoSelectionRadius", m_mcPfoSelectionRadius));

    m_mcPfoSelectionMomentum = 0.01f;
    PandoraReturnOnError(XmlHelper::ReadOptional(*pXmlHandle, "MCPfoSelectionMomentum", m_mcPfoSelectionMomentum));

    m_mcPfoSelectionLowEnergyNPCutOff = 1.2f;
    PandoraReturnOnError(XmlHelper::ReadOptional(*pXmlHandle, "MCPfoSelectionProtonNeutronEnergyCutOff", m_mcPfoSelectionLowEnergyNPCutOff));

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
