/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*     Parser for the WirelessVillage OTA settings.
*     Supports only wbxml.
*
*/

#include <e32std.h>
#include <xmlelemt.h>
#include <cnode.h>
#include "wvsettings.h"
#include "wvsettingsdef.h"

// CONSTANTS
const TInt KMaxStringSize	= 200;
const TInt KElemAndEndTokenLength	= 2;
const TInt KEndTokenLength	= 1;
// ---------------------------------------------------------
// CWVSettingsParser::CWVSettingsParser()
// ---------------------------------------------------------
//
CWVSettingsParser::CWVSettingsParser():
iListEmpty(ETrue),
iOTAVersion(EFalse),
iPassword(EFalse),
iPreferredProtocol(EFalse),
iProxyAddress(EFalse),
iProxyPort(EFalse),
iProxyUser(EFalse),
iProxyPassword(EFalse),
iRefName(EFalse),
iSAP4WaySupportedSms(EFalse),
iSAP4WaySupportedHttp(EFalse),
iSAPMobileNumber(EFalse),
iSAPURI(EFalse),
iSettingsName(EFalse),
iSMSCMobileNumber(EFalse),
iUserID(EFalse),
iSupportedVersions(EFalse),
iCommon(EFalse),
iSms(EFalse),
iHttp(EFalse),
iProxy(EFalse)
	{
	iDatabufferSize = 0;
	iParsedBytes = 0;
	}

// ---------------------------------------------------------
// CWVSettingsParser::NewL()
// ---------------------------------------------------------
//
EXPORT_C CWVSettingsParser* CWVSettingsParser::NewL()
	{
	CWVSettingsParser* self = new (ELeave) CWVSettingsParser ();
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop();
    return self;  
	}

// ---------------------------------------------------------
// CWVSettingsParser::ConstructL()
// ---------------------------------------------------------
//
void CWVSettingsParser::ConstructL()
	{
	iWVSupportedVersions = new (ELeave) CArrayPtrFlat<HBufC>(1);
	iRootElement = CXmlElement::NewL (&KWVSettings, NULL);
	iWVOTAVersion = HBufC::NewL(KMaxStringSize);
	iWVPassword = HBufC::NewL (KMaxStringSize);
	iWVPreferredProtocol = HBufC::NewL (KMaxStringSize);
	iWVProxyAddress = HBufC::NewL (KMaxStringSize);
	iWVProxyPort = HBufC::NewL (KMaxStringSize);
	iWVProxyUser = HBufC::NewL (KMaxStringSize);
	iWVRefName = HBufC::NewL (KMaxStringSize);
	iWVSAPFourWaySupported1 = HBufC::NewL (KMaxStringSize);
	iWVSAPFourWaySupported2 = HBufC::NewL (KMaxStringSize);
	iWVSAPMobileNumber = HBufC::NewL (KMaxStringSize);
	iWVSAPURI = HBufC::NewL (KMaxStringSize);
	iWVSettingsName = HBufC::NewL (KMaxStringSize);
	iWVSMSCMobileNumber = HBufC::NewL (KMaxStringSize);
	iWVUserID = HBufC::NewL (KMaxStringSize);
	iWVProxyPassword = HBufC::NewL (KMaxStringSize);
	}

// ---------------------------------------------------------
// CWVSettingsParser::~CWVSettingsParser()
// ---------------------------------------------------------
//
CWVSettingsParser::~CWVSettingsParser()
	{
	delete iRootElement;
	delete iWVSettingsName;
	delete iWVOTAVersion;
	delete iWVPassword;
	delete iWVPreferredProtocol;
	delete iWVProxyAddress; 
	delete iWVProxyPort;
	delete iWVProxyUser;
	delete iWVRefName;
	delete iWVSAPFourWaySupported1;
	delete iWVSAPFourWaySupported2;
	delete iWVSAPMobileNumber;
	delete iWVSAPURI;
	delete iWVSMSCMobileNumber;
	if (iWVSupportedVersions != NULL)
		{
		iWVSupportedVersions->ResetAndDestroy ();
		delete iWVSupportedVersions;
		}
	delete iWVUserID;	
	delete iWVProxyPassword;
	}

// ---------------------------------------------------------
// CWVSettingsParser::ParseL
// ---------------------------------------------------------
//
EXPORT_C TBool CWVSettingsParser::ParseL(TDesC8& aDataBuffer)
	{
	DoParseL (aDataBuffer);
	CreateXmlElementTreeL ();
	TBool errNotFound = Validate ();
	return errNotFound;
	}

// ---------------------------------------------------------
// CWVSettingsParser::CreateXmlElementTreeL
// ---------------------------------------------------------
//
void CWVSettingsParser::CreateXmlElementTreeL ()
	{
	/* used for the CleanupStack */
	TInt nbrOfElements = 0;
	/* Create all elements and set the data */
	if (iOTAVersion)
		{
		CXmlElement* WVOTAVersion = CXmlElement::NewL (&KWVOTAVersion, iRootElement);
		CleanupStack::PushL(WVOTAVersion);
		WVOTAVersion->SetTextL(*iWVOTAVersion);
		iRootElement->AppendChildL (WVOTAVersion);
		nbrOfElements++;
		}
	if (iCommon)
		{
		CXmlElement* WVCommon = CXmlElement::NewL (&KWVCommon, iRootElement);
		CleanupStack::PushL(WVCommon);
		if (iSettingsName)
			{
			CXmlElement* WVSettingsName = CXmlElement::NewL (&KWVSettingsName, WVCommon);
			CleanupStack::PushL(WVSettingsName);
			WVSettingsName->SetTextL(*iWVSettingsName);
			WVCommon->AppendChildL (WVSettingsName);
			nbrOfElements++;
			}
		if (iRefName)
			{
			CXmlElement* WVRefName = CXmlElement::NewL (&KWVRefName, WVCommon);
			CleanupStack::PushL(WVRefName);
			WVRefName->SetTextL(*iWVRefName);
			WVCommon->AppendChildL (WVRefName);
			nbrOfElements++;
			}
		if (iSupportedVersions)
			{
			CXmlElement* WVSupportedVersion;
			TInt nbrOfVersions = iWVSupportedVersions->Count();
			for (TInt i = 0; i < nbrOfVersions; i++)
				{
				WVSupportedVersion = CXmlElement::NewL (&KWVSupportedVersion, WVCommon);
				CleanupStack::PushL(WVSupportedVersion);
				WVSupportedVersion->SetTextL(*iWVSupportedVersions->At(i));
				WVCommon->AppendChildL (WVSupportedVersion);
				nbrOfElements++;
				}
			}
		if (iUserID)
			{
			CXmlElement* WVUserID = CXmlElement::NewL (&KWVUserID, WVCommon);
			CleanupStack::PushL(WVUserID);
			WVUserID->SetTextL(*iWVUserID);
			WVCommon->AppendChildL (WVUserID);
			nbrOfElements++;
			}
		if (iPassword)
			{
			CXmlElement* WVPassword = CXmlElement::NewL (&KWVPassword, WVCommon);
			CleanupStack::PushL(WVPassword);
			WVPassword->SetTextL(*iWVPassword);
			WVCommon->AppendChildL (WVPassword);
			nbrOfElements++;
			}
		if (iPreferredProtocol)
			{
			CXmlElement* WVPreferredProtocol = CXmlElement::NewL (&KWVPreferredProtocol, WVCommon);
			CleanupStack::PushL(WVPreferredProtocol);
			WVPreferredProtocol->SetTextL(*iWVPreferredProtocol);
			WVCommon->AppendChildL (WVPreferredProtocol);
			nbrOfElements++;
			}
		iRootElement->AppendChildL (WVCommon);
		nbrOfElements++;
		} //WVCommon
	if (iSms)
		{
		CXmlElement* WVSMS = CXmlElement::NewL (&KWVSMS, iRootElement);
		CleanupStack::PushL(WVSMS);
		if (iSAPMobileNumber)
			{
			CXmlElement* WVSAPMobileNumber = CXmlElement::NewL (&KWVSAPMobileNumber, WVSMS);
			CleanupStack::PushL(WVSAPMobileNumber);
			WVSAPMobileNumber->SetTextL(*iWVSAPMobileNumber);
			WVSMS->AppendChildL (WVSAPMobileNumber);
			nbrOfElements++;
		}
		if (iSAP4WaySupportedSms)
			{
			CXmlElement* WVSAPFourWaySupported1 = CXmlElement::NewL (&KWVSAPFourWaySupported, WVSMS);
			CleanupStack::PushL(WVSAPFourWaySupported1);
			WVSAPFourWaySupported1->SetTextL(*iWVSAPFourWaySupported1);
			WVSMS->AppendChildL (WVSAPFourWaySupported1);
			nbrOfElements++;
			}
		if (iSMSCMobileNumber)
			{
			CXmlElement* WVSMSCMobileNumber = CXmlElement::NewL (&KWVSMSCMobileNumber, WVSMS);
			CleanupStack::PushL(WVSMSCMobileNumber);
			WVSMSCMobileNumber->SetTextL(*iWVSMSCMobileNumber);
			WVSMS->AppendChildL (WVSMSCMobileNumber);
			nbrOfElements++;
			}
		iRootElement->AppendChildL (WVSMS);
		nbrOfElements++;
		} // WVSMS
	if (iHttp)
		{
		CXmlElement* WVHTTP = CXmlElement::NewL (&KWVHTTP, iRootElement);
		CleanupStack::PushL(WVHTTP);
		if ( iSAPURI)
			{
			CXmlElement* WVSAPURI = CXmlElement::NewL (&KWVSAPURI, WVHTTP);
			CleanupStack::PushL(WVSAPURI);
			WVSAPURI->SetTextL(*iWVSAPURI);
			WVHTTP->AppendChildL (WVSAPURI);
			nbrOfElements++;
			}
		if (iSAP4WaySupportedHttp)
			{
			CXmlElement* WVSAPFourWaySupported2 = CXmlElement::NewL (&KWVSAPFourWaySupported, WVHTTP);
			CleanupStack::PushL(WVSAPFourWaySupported2);
			WVSAPFourWaySupported2->SetTextL(*iWVSAPFourWaySupported2);
			WVHTTP->AppendChildL (WVSAPFourWaySupported2);
			nbrOfElements++;
			}
		if (iProxy)
			{
			CXmlElement* WVProxy = CXmlElement::NewL (&KWVProxy, WVHTTP);
			CleanupStack::PushL(WVProxy);
			if ( iProxyAddress)
				{
				CXmlElement* WVProxyAddress = CXmlElement::NewL (&KWVProxyAddress, WVProxy);
				CleanupStack::PushL(WVProxyAddress);
				WVProxyAddress->SetTextL(*iWVProxyAddress);
				WVProxy->AppendChildL (WVProxyAddress);
				nbrOfElements++;
				}
			if (iProxyPort)
				{
				CXmlElement* WVProxyPort = CXmlElement::NewL (&KWVProxyPort, WVProxy);
				CleanupStack::PushL(WVProxyPort);
				WVProxyPort->SetTextL(*iWVProxyPort);
				WVProxy->AppendChildL (WVProxyPort);
				nbrOfElements++;
				}
			if (iProxyUser)
				{
				CXmlElement* WVProxyUser = CXmlElement::NewL (&KWVProxyUser, WVProxy);
				CleanupStack::PushL(WVProxyUser);
				WVProxyUser->SetTextL(*iWVProxyUser);
				WVProxy->AppendChildL (WVProxyUser);
				nbrOfElements++;
				}
			if (iProxyPassword)
				{
				CXmlElement* WVProxyPassword = CXmlElement::NewL (&KWVProxyPassword, WVProxy);
				CleanupStack::PushL(WVProxyPassword);
				WVProxyPassword->SetTextL(*iWVProxyPassword);
				WVProxy->AppendChildL (WVProxyPassword);
				nbrOfElements++;
				}
			WVHTTP->AppendChildL (WVProxy);
			nbrOfElements++;
			} // WVProxy
		iRootElement->AppendChildL (WVHTTP);
		nbrOfElements++;
		} // WVHTTP
	//for testing
	//iRootElement->WriteIntoFileL();
	CleanupStack::Pop(nbrOfElements);  // elements
	}

// ---------------------------------------------------------
// CWVSettingsParser::Root ()
// ---------------------------------------------------------
//
EXPORT_C CXmlElement* CWVSettingsParser::Root ()
	{
	return iRootElement;
	}

// ---------------------------------------------------------
// CWVSettingsParser::DoParseL
// ---------------------------------------------------------
//
void CWVSettingsParser::DoParseL(TDesC8& aDataBuffer)
	{
	TBool done = EFalse;
	TUint8* dataPtr = (TUint8*) aDataBuffer.Ptr();

	iDatabufferSize = aDataBuffer.Size();

	if (iDatabufferSize > 0)
		{
		// save the buffer's start point
		TUint8* startPtr = dataPtr;
		//version, public id, charset
		dataPtr = dataPtr+3;
		//string table 
		dataPtr++;
		
		// this is used for 4waySupported
		TBool sms = EFalse;
		while(!done)
		{
			switch (*dataPtr)
			{
			case (KWV_TAG_CONTENT + K_WVSETTINGS):
				dataPtr++;
				break;
			case (KWV_TAG_CONTENT + K_WVOTAVERSION):	
				dataPtr = dataPtr + ReadElementWithContentL(dataPtr, iWVOTAVersion);
				iOTAVersion = ETrue;
			break;
			case (KWV_TAG_CONTENT + K_WVCOMMON):
				dataPtr++;
				iCommon = ETrue;
			break;
			case(KWV_TAG_CONTENT + K_WVSETTINGSNAME):
				dataPtr = dataPtr + ReadElementWithContentL(dataPtr, iWVSettingsName);
				iSettingsName = ETrue;
			break;
			case(KWV_TAG_CONTENT + K_WVREFNAME):   
				dataPtr = dataPtr + ReadElementWithContentL(dataPtr, iWVRefName);
				iRefName = ETrue;
			break;
			case (KWV_TAG_CONTENT + K_WVSUPPORTEDVERSION):
				dataPtr = dataPtr + ReadSupportedVersionsL(dataPtr);
				iSupportedVersions = ETrue;
			break;
			case (KWV_TAG_CONTENT + K_WVUSERID):
				dataPtr = dataPtr + ReadElementWithContentL(dataPtr, iWVUserID);
				iUserID = ETrue;
			break;
			case (KWV_TAG_CONTENT + K_WVPASSWORD):
				dataPtr = dataPtr + ReadElementWithContentL(dataPtr, iWVPassword);
				iPassword = ETrue;
			break;
			case (KWV_TAG_CONTENT + K_WVPREFERREDPROTOCOL):
				dataPtr = dataPtr + ReadElementWithContentL(dataPtr, iWVPreferredProtocol);
				iPreferredProtocol = ETrue;
			break;
			case (KWV_TAG_CONTENT + K_WVSMS):
				dataPtr++;
				sms = ETrue;
				iSms = ETrue;
			break;
			case (KWV_TAG_CONTENT + K_WVSAPMOBILENUMBER):
				dataPtr = dataPtr + ReadElementWithContentL(dataPtr, iWVSAPMobileNumber);
				iSAPMobileNumber = ETrue;
			break;
			case (KWV_TAG_CONTENT + K_WVSAPFOURWAYSUPPORTED):
				// NOTE This assumes that if msg has WVSMS element, it comes always before WVHTTP element
				// anyhow the specs say that...
				// ...The elements in the WV settings must be sent in a same order as the 
				// content models of these DTD elements define them
				if (sms)
					{
					dataPtr = dataPtr + ReadElementWithContentL(dataPtr, iWVSAPFourWaySupported1);
					sms = EFalse;
					iSAP4WaySupportedSms = ETrue;
					}
				else
					{
					dataPtr = dataPtr + ReadElementWithContentL(dataPtr, iWVSAPFourWaySupported2);
					iSAP4WaySupportedHttp = ETrue;
					}
			break;
			case (KWV_TAG_CONTENT + K_WVSMSMOBILENUMBER):
				dataPtr = dataPtr + ReadElementWithContentL(dataPtr, iWVSMSCMobileNumber);
				iSMSCMobileNumber = ETrue;
			break;
			case (KWV_TAG_CONTENT + K_WVHTTP):
				dataPtr++;
				iHttp = ETrue;
			break;
			case (KWV_TAG_CONTENT + K_WVSAPURI):
				dataPtr = dataPtr + ReadElementWithContentL(dataPtr, iWVSAPURI);
				iSAPURI = ETrue;
			break;
			case (KWV_TAG_CONTENT + K_WVPROXY):
				dataPtr++;
				iProxy = ETrue;
			break;
			case (KWV_TAG_CONTENT + K_WVPROXYADDRESS):
				dataPtr = dataPtr + ReadElementWithContentL(dataPtr, iWVProxyAddress);
				iProxyAddress = ETrue;
			break;
			case (KWV_TAG_CONTENT + K_WVPROXYPORT):
				dataPtr = dataPtr + ReadElementWithContentL(dataPtr, iWVProxyPort);
				iProxyPort = ETrue;
			break;
			case (KWV_TAG_CONTENT + K_WVPROXYUSER):
				dataPtr = dataPtr + ReadElementWithContentL(dataPtr, iWVProxyUser);
				iProxyUser = ETrue;
			break;
			case (KWV_TAG_CONTENT + K_WVPROXYPASSWORD):
				dataPtr = dataPtr + ReadElementWithContentL(dataPtr, iWVProxyPassword);
				iProxyPassword = ETrue;
			break;
			
			case K_WVOTAVERSION:
				dataPtr = dataPtr + ReadEmptyElement (dataPtr, iWVOTAVersion);
				iOTAVersion = ETrue;
			break;
			case K_WVCOMMON:
				dataPtr+=KElemAndEndTokenLength; 
				iCommon = ETrue;
			break;
			case K_WVSETTINGSNAME:
				dataPtr = dataPtr + ReadEmptyElement (dataPtr, iWVSettingsName);
				iSettingsName = ETrue;
			break;
			case K_WVREFNAME:   
				dataPtr = dataPtr + ReadEmptyElement (dataPtr, iWVRefName);
				iRefName = ETrue;
			break;
			case K_WVSUPPORTEDVERSION:
				dataPtr+=KElemAndEndTokenLength; 
				iSupportedVersions = ETrue;
			break;
			case K_WVUSERID:
				dataPtr = dataPtr + ReadEmptyElement (dataPtr, iWVUserID);
				iUserID = ETrue;
			break;
			case K_WVPASSWORD:
				dataPtr = dataPtr + ReadEmptyElement (dataPtr, iWVPassword);
				iPassword = ETrue;
			break;
			case K_WVPREFERREDPROTOCOL:
				dataPtr = dataPtr + ReadEmptyElement (dataPtr, iWVPreferredProtocol);
				iPreferredProtocol = ETrue;
			break;
			case K_WVSMS:
				dataPtr+=KElemAndEndTokenLength; 
				iSms = ETrue;
			break;
			case K_WVSAPMOBILENUMBER:
				dataPtr = dataPtr + ReadEmptyElement (dataPtr, iWVSAPMobileNumber);
				iSAPMobileNumber = ETrue;
			break;
			case K_WVSAPFOURWAYSUPPORTED:  
				if (sms)
					{
					dataPtr = dataPtr + ReadEmptyElement (dataPtr, iWVSAPFourWaySupported1);
					sms = EFalse;
					iSAP4WaySupportedSms = ETrue;
					}
				else
					{
					dataPtr = dataPtr + ReadEmptyElement (dataPtr, iWVSAPFourWaySupported2);
					iSAP4WaySupportedHttp = ETrue;
					}
			break;
			case K_WVSMSMOBILENUMBER:
				dataPtr = dataPtr + ReadEmptyElement (dataPtr, iWVSMSCMobileNumber);
				iSMSCMobileNumber = ETrue;
			break;
			case K_WVHTTP:
				dataPtr+=KElemAndEndTokenLength; 
				iHttp = ETrue;
			break;
			case K_WVSAPURI:
				dataPtr = dataPtr + ReadEmptyElement (dataPtr, iWVSAPURI);
				iSAPURI = ETrue;
			break;
			case K_WVPROXY:
				dataPtr+=KElemAndEndTokenLength; 
				iProxy = ETrue;
			break;
			case K_WVPROXYADDRESS:
				dataPtr = dataPtr + ReadEmptyElement (dataPtr, iWVProxyAddress);
				iProxyAddress = ETrue;
			break;
			case K_WVPROXYPORT:
				dataPtr = dataPtr + ReadEmptyElement (dataPtr, iWVProxyPort);
				iProxyPort = ETrue;
			break;
			case K_WVPROXYUSER:
				dataPtr = dataPtr + ReadEmptyElement (dataPtr, iWVProxyUser);
				iProxyUser = ETrue;
			break;
			case K_WVPROXYPASSWORD:
				dataPtr = dataPtr + ReadEmptyElement (dataPtr, iWVProxyPassword);
				iProxyPassword = ETrue;
			break;
			case KWV_END_TAG:
				dataPtr++; // end tag
			break;
				default:
					dataPtr++; 
				break;
			}//switch
			iParsedBytes = dataPtr - startPtr;
			if ( iParsedBytes >= iDatabufferSize)
				done = ETrue;
			}//while
		} // databuffer

}

// ---------------------------------------------------------
// CWVSettingsParser::ReadElementWithContentL
// ---------------------------------------------------------
//
TInt CWVSettingsParser::ReadElementWithContentL(TUint8* aPtr, HBufC* aDataBuffer)
	{
	TInt count = 0;
	TInt inlineStr = 0;
	// remove the element tag
	aPtr++;
	//next should be inline string token 0x03
	if (*aPtr == KWV_STR_I){
		aPtr++;
		inlineStr++;
	}
	// find the end tag
	TUint8* start = aPtr;
	while ( *start != KWV_END_TAG && ((iParsedBytes + inlineStr + count) < iDatabufferSize))
		{
		count++;
		start++;
		}
	TInt size = 0;
	if (count > KMaxStringSize)
		size = KMaxStringSize;
	else if (count == 0)
		size = 0;
	else
		size = count-1;
	aDataBuffer->ReAllocL(size);
	TPtr16 dataPtr = aDataBuffer->Des();
	for (TInt i = 0; i < size; i++)
		{
		dataPtr.Append(*aPtr);
		aPtr++;
		}
	return count + inlineStr + KElemAndEndTokenLength;  
	}

// ---------------------------------------------------------
// CWVSettingsParser::ReadSupportedVersionsL
// ---------------------------------------------------------
//
TInt CWVSettingsParser::ReadSupportedVersionsL(TUint8* aPtr)
	{
	TInt count = 0;
	TInt inlineStr = 0;
	// remove the element tag
	aPtr++;
	//next should be inline string token 0x03
	if (*aPtr == KWV_STR_I){
		aPtr++;
		inlineStr++;
	}
	// find the end tag
	TUint8* start = aPtr;
	while ( *start != KWV_END_TAG && ((iParsedBytes + inlineStr + count) < iDatabufferSize))
		{
		count++;
		start++;
		}
	TInt size = 0;
	if (count > KMaxStringSize)
		size = KMaxStringSize;
	else if (count == 0)
		size = 0;
	else
		size = count-1;
	HBufC* version = HBufC::NewL(size);
	CleanupStack::PushL(version);
	TPtr16 dataPtr = version->Des();
	for (TInt i = 0; i < size ; i++)
		{
		dataPtr.Append(*aPtr);
		aPtr++;
		}
	if(iListEmpty)
		{
		iWVSupportedVersions->AppendL(version);
		iListEmpty = EFalse;
		}
	else
		{
		TInt index = iWVSupportedVersions->Count();
		iWVSupportedVersions->InsertL(index, version);
		}
	CleanupStack::Pop();
	return count + inlineStr + KElemAndEndTokenLength;  
}

// ---------------------------------------------------------
// CWVSettingsParser::ReadEmptyElement
// ---------------------------------------------------------
//
TInt CWVSettingsParser::ReadEmptyElement(TUint8* aPtr, HBufC* dataBuffer)
{
	TInt count = 0;
	TUint8* start = aPtr;

	while ( *start != KWV_END_TAG && ((iParsedBytes + count) <= iDatabufferSize))
		{
		count++;
		start++;
		}
	if (dataBuffer)
		dataBuffer = NULL;

	return count + KEndTokenLength;  // also the element tag
}

// ---------------------------------------------------------
// CWVSettingsParser::Validate ()
// ---------------------------------------------------------
//
TBool CWVSettingsParser::Validate ()
{
	TBool errNotFound = ETrue;
	// mandatory in wvsettings
	if (!iOTAVersion)
		errNotFound = EFalse;
	// mandatory in wvsettings
	if (!iCommon)
		errNotFound = EFalse;
	else
		{
		// mandatory in wvcommon
		if (!iSettingsName)
			errNotFound = EFalse;
		// mandatory in wvcommon
		if (!iSupportedVersions)
			errNotFound = EFalse;
		}
	if (iSms)
		{
		// mandatory in wvsms
		if (!iSAPMobileNumber)
			errNotFound = EFalse;
		// mandatory in wvsms
		if (!iSAP4WaySupportedSms)
			errNotFound = EFalse;
		if (!iSMSCMobileNumber)
		// mandatory in wvsms
			errNotFound = EFalse;
		}
	if (iHttp)
		{
		// mandatory in wvhttp
		if (!iSAPURI)
			errNotFound = EFalse;
		// mandatory in wvhttp
		if (!iSAP4WaySupportedHttp)
			errNotFound = EFalse;
		// optional in wvhttp
		if (iProxy)
			{
			// mandatory in wvproxy
			if (!iProxyAddress)
				errNotFound = EFalse;
			}
		}
	return errNotFound;  
	}
