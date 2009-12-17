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
*			 WirelessVillage OTA settings parser.
*
*/


#ifndef CWVSettings_H
#define CWVSettings_H

//  INCLUDES
#include <e32base.h>
#include <xmlelemt.h>
#include <cnode.h>

class CWVSettingsParser :public CBase 
    {
    public:  // Constructors and destructor
		
   		/**
        * Two-phased constructor.
        */
        IMPORT_C static CWVSettingsParser* NewL ();
        /**
        * Destructor
        */
		virtual ~CWVSettingsParser();

    public: // New functions

		/**
        * Parses the data
        * @param aDataBuffer contains the data 
		* @return EFalse, if mandatory value is missing, otherwise ETrue
        */
		IMPORT_C TBool ParseL(TDesC8& aDataBuffer);	
		/**
        * Accessor to the root element (WVSettings)
        * @return CXmlElement*, pointer to the root element 
        */
		IMPORT_C CXmlElement* Root ();	
		/**
        * Parses the data, sets values to the internal data structure
        * @param aDataPtr pointer to the data 
        */
		void DoParseL(TDesC8& aDataBuffer);
		/**
        * Read element, sets values to the internal data structure
        * @param aDataPtr pointer to the data 
		* @return number of parsed bytes 
        */
		TInt ReadElementWithContentL(TUint8* ptr, HBufC* aDataBuffer);
		/**
        * Read empty element
        * @param aDataPtr pointer to the data 
		* @return number of parsed bytes 
        */
		TInt ReadEmptyElement(TUint8* aDataPtr, HBufC* aDataBuffer);
		/**
        * Parses the data, sets values to the internal data structure
        * @param aDataPtr pointer to the data 
		* @return number of parsed bytes 
        */
		TInt ReadSupportedVersionsL(TUint8* aDataPtr);
		/**
        * Creates xml tree
        */
		void CreateXmlElementTreeL ();
		/**
        * Validates the data
        * @return EFalse, if mandatory value is missing, otherwise ETrue
        */
		TBool Validate ();

    private:
		/**
        * C++ default constructor.
        */
		CWVSettingsParser();
		/**
        * Actual construction.
        */
		void ConstructL ();

    private:    // Data
		
		TInt iDatabufferSize;
		TInt iParsedBytes;
		CXmlElement* iRootElement;
		HBufC* iWVOTAVersion;
		HBufC* iWVPassword;
		HBufC* iWVPreferredProtocol;
		HBufC* iWVProxyAddress;
		HBufC* iWVProxyPort;
		HBufC* iWVProxyUser;
		HBufC* iWVProxyPassword;
		HBufC* iWVRefName;
		HBufC* iWVSAPFourWaySupported1;
		HBufC* iWVSAPFourWaySupported2;
		HBufC* iWVSAPMobileNumber;
		HBufC* iWVSAPURI;
		HBufC* iWVSettingsName;
		HBufC* iWVSMSCMobileNumber;
		HBufC* iWVUserID;	
		CArrayPtrFlat<HBufC>* iWVSupportedVersions;
		TBool iListEmpty;
		// these are used for validating
		// and xml tree creation
		TBool iOTAVersion;
		TBool iPassword;
		TBool iPreferredProtocol;
		TBool iProxyAddress;
		TBool iProxyPort;
		TBool iProxyUser;
		TBool iProxyPassword;
		TBool iRefName;
		TBool iSAP4WaySupportedSms;
		TBool iSAP4WaySupportedHttp;
		TBool iSAPMobileNumber;
		TBool iSAPURI;
		TBool iSettingsName;
		TBool iSMSCMobileNumber;
		TBool iUserID;	
		TBool iSupportedVersions;
		TBool iCommon;
		TBool iSms;
		TBool iHttp;
		TBool iProxy;
    };

#endif     
            
// End of File
