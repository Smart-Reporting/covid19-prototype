#include "stripDataset.h"
#include "utils.h"
#include <dcmtk/dcmdata/dcelem.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <iostream>
#include <list>
#include <unordered_set>

// Hash operation for DcmTagKey class
struct dcmtagkey_hash
{
    size_t operator()(const DcmTagKey& tag) const
    {
        return tag.hash();
    }
};
// List of confidential tags according to 
// Annex E (Attribute Confidentiality Profiles)
// of the dicom specification.
// See http://dicom.nema.org/Medical/Dicom/2014a/output/chtml/part15/chapter_E.html
static const std::unordered_set<DcmTagKey, dcmtagkey_hash> confidentialTags =
{
    DCM_AccessionNumber,
    DcmTagKey(0x18,0x4000), //DCM_RETIRED_AcquisitionComments,
    DCM_AcquisitionContextSequence,
    DCM_AcquisitionDate,
    DCM_AcquisitionDateTime,
    DCM_AcquisitionDeviceProcessingDescription,
    DCM_AcquisitionProtocolDescription,
    DCM_AcquisitionTime,
    DCM_ActualHumanPerformersSequence,
    DCM_AdditionalPatientHistory,
    DCM_RETIRED_AddressTrial,
    DCM_AdmissionID,
    DCM_AdmittingDate,
    DCM_AdmittingDiagnosesCodeSequence,
    DCM_AdmittingDiagnosesDescription,
    DCM_AdmittingTime,
    DCM_AffectedSOPInstanceUID,
    DCM_Allergies,
    DcmTagKey(0x4000,0x10), //DCM_RETIRED_Arbitrary,
    DCM_AuthorObserverSequence,
    DCM_BranchOfService,
    DCM_CassetteID,
    DCM_CommentsOnThePerformedProcedureStep,
    DCM_ConcatenationUID,
    DCM_ConfidentialityConstraintOnPatientDataDescription,
    DCM_ContentCreatorName,
    DCM_ContentCreatorIdentificationCodeSequence,
    DCM_ContentDate,
    DCM_ContentSequence,
    DCM_ContentTime,
    DCM_ContextGroupExtensionCreatorUID,
    DCM_ContrastBolusAgent,
    DCM_ContributionDescription,
    DCM_CountryOfResidence,
    DCM_CreatorVersionUID,
    DcmTagKey(0x40,0xa307), // DCM_RETIRED_CurrentObserverTrial,
    DCM_CurrentPatientLocation,
    DCM_RETIRED_CurveData,
    DCM_RETIRED_CurveDate,
    DCM_RETIRED_CurveTime,
    DCM_CustodialOrganizationSequence,
    DCM_DataSetTrailingPadding,
    DCM_DerivationDescription,
    DCM_DetectorID,
    DCM_DeviceSerialNumber,
    DCM_DeviceUID,
    DCM_DigitalSignatureUID,
    DcmTagKey(0xFFFA,0xFFFA), // Digital signature sequence
    DCM_DimensionOrganizationUID,
    DCM_RETIRED_DischargeDiagnosisDescription,
    DCM_RETIRED_DistributionAddress,
    DCM_RETIRED_DistributionName,
    DCM_DoseReferenceUID,
    DCM_EndAcquisitionDateTime,
    DCM_EthnicGroup,
    DCM_ExpectedCompletionDateTime,
    DCM_FailedSOPInstanceUIDList,
    DCM_FiducialUID,
    DCM_FillerOrderNumberImagingServiceRequest,
    DCM_FrameComments,
    DCM_FrameOfReferenceUID,
    DCM_GantryID,
    DCM_GeneratorID,
    DCM_GraphicAnnotationSequence,
    DCM_HumanPerformerName,
    DCM_HumanPerformerOrganization,
    DCM_IconImageSequence,
    DcmTagKey(0x8,0x4000), //DCM_RETIRED_IdentifyingComments,
    DCM_ImageComments,
    DcmTagKey(0x28,0x4000), //DCM_RETIRED_ImagePresentationComments,
    DCM_ImagingServiceRequestComments,
    DCM_RETIRED_Impressions,
    DcmTagKey(0x8,0x15), //DCM_InstanceCoercionDateTime,
    DCM_InstanceCreatorUID,
    DCM_InstitutionAddress,
    DCM_InstitutionCodeSequence,
    DCM_InstitutionName,
    DCM_InstitutionalDepartmentName,
    DcmTagKey(0x10,0x1050), //DCM_RETIRED_InsurancePlanIdentification,
    DCM_IntendedRecipientsOfResultsIdentificationSequence,
    DCM_RETIRED_InterpretationApproverSequence,
    DCM_RETIRED_InterpretationAuthor,
    DCM_RETIRED_InterpretationDiagnosisDescription,
    DCM_RETIRED_InterpretationIDIssuer,
    DCM_RETIRED_InterpretationRecorder,
    DCM_RETIRED_InterpretationText,
    DCM_RETIRED_InterpretationTranscriber,
    DCM_IrradiationEventUID,
    DCM_RETIRED_IssuerOfAdmissionID,
    DCM_IssuerOfPatientID,
    DCM_RETIRED_IssuerOfServiceEpisodeID,
    DCM_RETIRED_LargePaletteColorLookupTableUID,
    DCM_LastMenstrualDate,
    DCM_MAC,
    DCM_MediaStorageSOPInstanceUID,
    DCM_MedicalAlerts,
    DcmTagKey(0x10,0x1090), //DCM_RETIRED_MedicalRecordLocator,
    DCM_MilitaryRank,
    DCM_ModifiedAttributesSequence,
    DcmTagKey(0x20,0x3406), //DCM_RETIRED_ModifiedImageDescription,
    DcmTagKey(0x20,0x3401), //DCM_RETIRED_ModifyingDeviceID,
    DcmTagKey(0x20,0x3404), //DCM_RETIRED_ModifyingDeviceManufacturer,
    DCM_NameOfPhysiciansReadingStudy,
    DCM_NamesOfIntendedRecipientsOfResults,
    DcmTagKey(0x40,0xa192), //DCM_RETIRED_ObservationDateTrial,
    DcmTagKey(0x40,0xa402), //DCM_RETIRED_ObservationSubjectUIDTrial,
    DcmTagKey(0x40,0xa193), //DCM_RETIRED_ObservationTimeTrial,
    DcmTagKey(0x40,0xa171), //DCM_ObservationUID,
    DCM_Occupation,
    DCM_OperatorIdentificationSequence,
    DCM_OperatorsName,
    DCM_OriginalAttributesSequence,
    DCM_OrderCallbackPhoneNumber,
    DCM_OrderEnteredBy,
    DCM_OrderEntererLocation,
    DcmTagKey(0x10,0x1000), //DCM_RETIRED_OtherPatientIDs,
    DCM_OtherPatientIDsSequence,
    DCM_OtherPatientNames,
    DcmTagKey(0x6000,0x4000), //DCM_RETIRED_OverlayComments,
    DCM_OverlayData,
    DCM_RETIRED_OverlayDate,
    DCM_RETIRED_OverlayTime,
    DCM_PaletteColorLookupTableUID,
    DCM_ParticipantSequence,
    DCM_PatientAddress,
    DCM_PatientComments,
//    DCM_PatientID,
    DCM_PatientSexNeutered,
    DCM_PatientState,
    DCM_PatientTransportArrangements,
    DCM_PatientAge,
    DCM_PatientBirthDate,
    DCM_PatientBirthName,
    DCM_PatientBirthTime,
    DCM_PatientInstitutionResidence,
    DCM_PatientInsurancePlanCodeSequence,
    DCM_PatientMotherBirthName,
    DCM_PatientName,
    DCM_PatientPrimaryLanguageCodeSequence,
    DCM_PatientPrimaryLanguageModifierCodeSequence,
    DCM_PatientReligiousPreference,
    DCM_PatientSex,
    DCM_PatientSize,
    DCM_PatientTelephoneNumbers,
    DCM_PatientWeight,
    DCM_PerformedLocation,
    DCM_PerformedProcedureStepDescription,
    DCM_PerformedProcedureStepEndDate,
    DcmTagKey(0x0040, 0x4051), //DCM_PerformedProcedureStepEndDateTime,
    DCM_PerformedProcedureStepEndTime,
    DCM_PerformedProcedureStepID,
    DCM_PerformedProcedureStepStartDate,
    DcmTagKey(0x40,0x4050), //DCM_PerformedProcedureStepStartDateTime,
    DCM_PerformedProcedureStepStartTime,
    DCM_PerformedStationAETitle,
    DCM_PerformedStationGeographicLocationCodeSequence,
    DCM_PerformedStationName,
    DCM_PerformedStationNameCodeSequence,
    DCM_PerformingPhysicianIdentificationSequence,
    DCM_PerformingPhysicianName,
    DCM_PersonAddress,
    DCM_PersonIdentificationCodeSequence,
    DCM_PersonName,
    DCM_PersonTelephoneNumbers,
    DCM_RETIRED_PhysicianApprovingInterpretation,
    DCM_PhysiciansReadingStudyIdentificationSequence,
    DCM_PhysiciansOfRecord,
    DCM_PhysiciansOfRecordIdentificationSequence,
    DCM_PlacerOrderNumberImagingServiceRequest,
    DCM_PlateID,
    DCM_PreMedication,
    DCM_PregnancyStatus,
    DcmTagKey(0x40,0x4052), //DCM_ProcedureStepCancellationDateTime,
    DCM_ProtocolName,
    DCM_RETIRED_ReasonForTheImagingServiceRequest,
    DCM_RETIRED_ReasonForStudy,
    DCM_ReferencedDigitalSignatureSequence,
    DCM_ReferencedFrameOfReferenceUID,
    DcmTagKey(0x40,0x4023), //DCM_RETIRED_ReferencedGeneralPurposeScheduledProcedureStepTransactionUID,
    DCM_ReferencedImageSequence,
    DcmTagKey(0x40,0xa172), //DCM_RETIRED_ReferencedObservationUIDTrial,
    DCM_ReferencedPatientAliasSequence,
    DcmTagKey(0x10,0x1100), //DCM_ReferencedPatientPhotoSequence,
    DCM_ReferencedPatientSequence,
    DCM_ReferencedPerformedProcedureStepSequence,
    DCM_ReferencedSOPInstanceMACSequence,
    DCM_ReferencedSOPInstanceUID,
    DCM_ReferencedSOPInstanceUIDInFile,
    DCM_ReferencedStudySequence,
    DCM_ReferringPhysicianAddress,
    DCM_ReferringPhysicianIdentificationSequence,
    DCM_ReferringPhysicianName,
    DCM_ReferringPhysicianTelephoneNumbers,
    DCM_RegionOfResidence,
    DcmTagKey(0x3006,0x00c2), //DCM_RETIRED_RelatedFrameOfReferenceUID,
    DCM_RequestAttributesSequence,
    DCM_RequestedContrastAgent,
    DCM_RequestedProcedureComments,
    DCM_RequestedProcedureDescription,
    DCM_RequestedProcedureID,
    DCM_RequestedProcedureLocation,
    DCM_RequestedSOPInstanceUID,
    DCM_RequestingPhysician,
    DCM_RequestingService,
    DCM_ResponsibleOrganization,
    DCM_ResponsiblePerson,
    DCM_RETIRED_ResultsComments,
    DCM_RETIRED_ResultsDistributionListSequence,
    DCM_RETIRED_ResultsIDIssuer,
    DCM_ReviewerName,
    DCM_ScheduledHumanPerformersSequence,
    DCM_RETIRED_ScheduledPatientInstitutionResidence,
    DCM_ScheduledPerformingPhysicianIdentificationSequence,
    DCM_ScheduledPerformingPhysicianName,
    DCM_ScheduledProcedureStepEndDate,
    DCM_ScheduledProcedureStepEndTime,
    DCM_ScheduledProcedureStepDescription,
    DCM_ScheduledProcedureStepLocation,
    DCM_ScheduledProcedureStepModificationDateTime,
    DCM_ScheduledProcedureStepStartDate,
    DCM_ScheduledProcedureStepStartDateTime,
    DCM_ScheduledProcedureStepStartTime,
    DCM_ScheduledStationAETitle,
    DCM_ScheduledStationGeographicLocationCodeSequence,
    DCM_ScheduledStationName,
    DCM_ScheduledStationNameCodeSequence,
    DCM_RETIRED_ScheduledStudyLocation,
    DCM_RETIRED_ScheduledStudyLocationAETitle,
    DCM_SeriesDate,
    DCM_SeriesDescription,
//    DCM_SeriesInstanceUID,
    DCM_SeriesTime,
    DCM_ServiceEpisodeDescription,
    DCM_ServiceEpisodeID,
    DCM_SmokingStatus,
//    DCM_SOPInstanceUID,
    DCM_SourceImageSequence,
    DCM_SourceSerialNumber,
    DCM_SpecialNeeds,
    DCM_StartAcquisitionDateTime,
    DCM_StationName,
    DCM_StorageMediaFileSetUID,
    DCM_RETIRED_StudyComments,
    DCM_StudyDate,
    DCM_StudyDescription,
    DCM_StudyID,
    DCM_RETIRED_StudyIDIssuer,
//    DCM_StudyInstanceUID,
    DCM_StudyTime,
    DCM_SynchronizationFrameOfReferenceUID,
    DcmTagKey(0x18,0x2042), //DCM_TargetUID,
    DCM_RETIRED_TelephoneNumberTrial,
    DCM_RETIRED_TemplateExtensionCreatorUID,
    DCM_RETIRED_TemplateExtensionOrganizationUID,
    DcmTagKey(0x4000,0x4000), //DCM_RETIRED_TextComments,
    DCM_TextString,
    DCM_TimezoneOffsetFromUTC,
    DCM_RETIRED_TopicAuthor,
    DCM_RETIRED_TopicKeywords,
    DCM_RETIRED_TopicSubject,
    DCM_RETIRED_TopicTitle,
    DCM_TransactionUID,
    DCM_UID,
    DcmTagKey(0x40,0xa352), //DCM_RETIRED_VerbalSourceTrial,
    DcmTagKey(0x40,0xa358), //DCM_RETIRED_VerbalSourceIdentifierCodeSequenceTrial,
    DCM_VerifyingObserverIdentificationCodeSequence,
    DCM_VerifyingObserverName,
    DCM_VerifyingObserverSequence,
    DCM_VerifyingOrganization,
    DCM_VisitComments
};


// Checks if a DICOM tag key belongs to
// confidential data or not
bool isConfidential(const DcmTagKey& key)
{
    // check if it is at private key
    bool confidential = key.isPrivate();
    Uint16 group = key.getGroup();
    Uint16 element = key.getElement();
    // Check it the key belongs to the curve data set
    if(!confidential && (group>=0x5000 && group<0x5100))
    {
        confidential = true;
    }
    // Check it the key belongs to the overlay comments
    if(!confidential && (group>=0x6000 && group<0x6100 && element==0x4000))
    {
        confidential = true;
    }
    // Check it the key belongs to the overlay data
    if(!confidential && (group>=0x6000 && group<0x6100 && element==0x3000))
    {
        confidential = true;
    }
    if(!confidential)
    {
        confidential = confidentialTags.find(key) != confidentialTags.end();
    }
    return confidential;
}

// Remove all confidential DICOM tags from a dcmtk item 
static OFCondition stripPrivateTags(
    DcmItem* item,   // item to be processed
    int tracingLevel // tracing level: 0,1 or 2 
    )
{
    if(!item)
    {
        return dcmtkError("Invalid DICOM item.");
    }

    if(tracingLevel>0)
    {
        std::cout << "Begin of removing DICOM tags" << std::endl;
    }
    // collect tag keys for deletion
    std::list<DcmTagKey> keysToDelete;
    for (unsigned long i = 0; i < item->card(); i++)
    {
        DcmElement* element = item->getElement(i);
        if (element != NULL)
        {
            const DcmTagKey key = element->getTag();
            if(isConfidential(key))
            {
                keysToDelete.push_back(key);
            }
        }
    }
    if(tracingLevel>0)
    {
        std::cout << "Number of tags to be deleted:" << keysToDelete.size() << std::endl;
    }
    // delete collected keys
    int countDeleted(0);
    for(const auto& key : keysToDelete)
    {
        OFCondition result = item->findAndDeleteElement(key, true, true);
        if(result.good())
        {
            countDeleted++;
            if(tracingLevel>1)
            {
                std::cout << "Successfuly deleted tag (" 
                    << std::hex 
                    << key.getGroup() 
                    <<  "," 
                    << key.getElement() 
                    << ")" 
                    << std::endl;
            }
        }
        else
        {
            std::cout << "Error by deletion of the tag (" 
                << std::hex 
                << key.getGroup() 
                <<  "," 
                << key.getElement() 
                << ")" 
                << std::endl;
            std::cout << result.text() << std::endl;
        }
    }    

    if(tracingLevel>0)
    {
        std::cout << "Number of deleted DICOM tags:" 
            << std::dec 
            << countDeleted
            << std::endl;
        std::cout << "End of removing DICOM tags" << std::endl;
    }
    OFCondition res;
    return res;
}

// Strips all confidential tags from a DICOM file
OFCondition stripPrivateTags(
    std::shared_ptr<DcmFileFormat> file, // dcmtk instance of DICOM file
    int tracingLevel                     // tracing level (0, 1, 2)
    )
{
    // Strip dataset
    DcmDataset* dataset = file->getDataset();
    if(!dataset)
    {
        return dcmtkError("Invalid DICOM dataset");
    }
    OFCondition res1 = stripPrivateTags(dataset, tracingLevel);
    if(res1.good())
    {
        return res1;
    }
    // strip file metainfo
    DcmMetaInfo* metainfo = file->getMetaInfo();
    if(!metainfo)
    {
        return dcmtkError("Invalid DICOM meta info item");
    }
    return stripPrivateTags(metainfo, tracingLevel);
}