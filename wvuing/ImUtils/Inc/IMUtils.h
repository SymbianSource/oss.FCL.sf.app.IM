/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  General utitity services for IM UI
*
*/



#ifndef IMUTILS_H
#define IMUTILS_H

//  INCLUDES
#include    <badesca.h>
#include    "impsbuilddefinitions.h"

// FORWARD DECLARATIONS
class CEikMenuPane;
class CGulIcon;
// The settings can be in Cenrep/Resource file, we need
// to have an instance of CRepository Class
class CRepository;


// CLASS DECLARATION
// 401-1815 Ease of Instant Messaging branding
// Code added for the above requirment

/**
* IM Utils internal implementation.
* This object is stored in TLS.
*
* @lib imutilsng.lib
* @since -
*/

class CIMUtilsTlsData : public CBase
    {


    private:
        /**
         * 401-1815 Ease of Instant Messaging branding
         * Code added for the above requirment
         *Default Constructor
         */
        CIMUtilsTlsData();

        /**
        * Symbian 2nd phase constructor can leave.
        */
        void ConstructL();


    public:  // Constructors and destructor

        /**
        * Code added for the above requirment

        * Two-phased constructor.
        * NewL
        */
        static CIMUtilsTlsData* NewL();

        /**
        * Code added for the above requirment
        * Destructor.
        */
        virtual ~CIMUtilsTlsData();


        /**
        * Returns the CRepository instance
        * @retrun CRepository - Returns reference of CRep
        */
        CRepository& RepositoryInstance() ;

    private:

        //Instance that holds the iRepository
        CRepository *iRepository;
    };
// End of code 401-1815 Ease of Instant Messaging branding



/**
 *  General utitity services for IM UI
 *
 *  @lib imutilsng.lib
 *  @since
 */
class IMUtils
    {
    public:
        enum TIgnoreOfflineStatus
            {
            EVariationDoesNotAllow = -3, // -2 equals to EAknSoftkeyOk constant,
            // that's why -3 is used here
            EUserCanceled = -1,
            EIgnoredByUser = 1,
            EContactOnline = 2
            };
    public:

        /**
         * Formats then given raw online-list to be compatible with list query
         * dialogs
         * @param aRawOnlineList Raw data (source)
         * @param aFormattedOnlineList Formatted data (target)
         */
        IMPORT_C static void PopulateFormattedOnlineListL(
            const MDesCArray& aRawOnlineList,
            CDesCArray& aFormattedOnlineList );

        /**
         * Globally used help launcher method.
         * @param aContext The help context
         */
        IMPORT_C static void LaunchHelpL( const TDesC& aContext );

        /**
         * Gets Chat login value from shared data
         * @return chat login type value in wvsettings shared data
         */
        IMPORT_C static TInt WVSettingsChatLoginValue();

        /**
         * Find if user is on userlist using NeutralCompare
         * @param aUserList list of wvid's
         * @param aUserId   user id to find
         * @return position of founding, KErrNotFound if not found
         */
        IMPORT_C static TInt NeutralFind( const MDesCArray& aUserList, const TDesC& aUserId );

        /**
         * 401-1815 Ease of Instant Messaging branding
         * Code added for the above requirment
         * Reads integer resource value from resources from Cenrep / Resource file
         * This function should be used if the caller is not sure if the setting
         * requested for is in cenrep/resource file.
         * @param aResourceId resource id which is read
         * @return value of wanted integer resource flag
         */
        IMPORT_C static TInt IntResourceValueL( TInt aResourceId );


        /**
         * 401-1815 Ease of Instant Messaging branding
         * Code added for the above requirment
         * Reads integer resource value only from resource file.
         * This function should be used only if the caller is sure that
         * the resource is present only in Resource file and not in Cenrep.
         * @param aResourceId resource id which is read
         * @return value of wanted integer resource flag
         */
        IMPORT_C static TInt IntResourceValueFromRssL( TInt aResourceId );

        /**
         * @return The maximum length for one message in characters
         */
        IMPORT_C static TInt MaxMsgLength();

        /**
         * @param aUid Uid for cenrep
         * @param aKey Central repository key
         * @return Value for key
         */
        IMPORT_C static TUint32 CRKeyL( TUid aUid, TUint32 aKey );

        /**
         * Adds menu item dynamically to menupane
         * @param aAdd If EFalse, this method doesn't add anything
         * @param aMenuPane Menu pane
         * @param aTextResourceId Resource id for menu item text
         * @param aCommandId Command id for menu item
         * @param aCascadeId Cascade menu resource id
         */
        IMPORT_C static void AddMenuItemL( TBool aAdd, CEikMenuPane& aMenuPane, TInt aTextResourceId,
                                           TInt aCommandId, TInt aCascadeId = 0 );

        /**
         * Sets bitmap/mask from source to target and set externality too
         * @param aTarget target for icon
         * @param aSource source for icon
         */
        IMPORT_C static void SetIcon( CGulIcon& aTarget, CGulIcon& aSource );

        /**
         * It assigns source as a target if target is NULL or
         * set target like source and delete source
         * WARNING source is expected on the top of the CleanupStack
         * source is removed from the Cleanupstack
         *
         * @param aTarget target for icon
         * @param aSource source for icon
         */
        IMPORT_C static void UpdateIcon( CGulIcon*& aTarget, CGulIcon* aSource );

        /**
         * Shows needed notes about offline status accordign to variation.
         * @precond aContactIdentification is identification of an offline contact.
         * @param aContactIdentification Contact identification.
         * @return Integer value, negative if offline status is not ignored or
         *              allowed by variation, positive if contact is online
         *              or user decides to ignore offline status. See possible
         *              return values from IMUtils::TIgnoreOfflineStatus enum.
         */
        IMPORT_C static TInt IgnoreOfflineStatusL( const TDesC& aContactIdentification );

        /**
         * Checks flash memory level. Leaves with KErrDiskFull if
         * memory level below critical level.
         * @since
         * @param aRequestedSpace Needed memory estimate. Default 0 checks
         *        if memory level is already below critical level.
         * @param aEmulateMemOut Can be used in DEBUG BUILDS to emulate memory
         *        out situation, ETrue causes the method to leave
         *        with KErrDiskfull.
         */
        IMPORT_C static void FFSSpaceBelowCriticalLevelL( TInt aRequestedSpace = 0
                                                                                 , TBool
#ifdef _DEBUG
                                                          aEmulateMemOut
#endif // _DEBUG
                                                          = EFalse
                                                        );

        /**
         * Checks if the file is DRM protected or not
         * @since
         * @param aFileName Name of the file to be checked
         */
        IMPORT_C static TBool FileProtectedL( const TFileName& aFileName );

        /**
         * Checks if the content is DRM protected or not
         *
         * Note that this method might be quite slow and should
         * not be used often!
         *
         * @since
         * @param aContent Content to be checked
         */
        IMPORT_C static TBool ContentProtectedL( const TDesC8& aContent );


        /**
         * Combines two strings given resource ids.
         *
         *
         * The caller is responsible to delete the string
         *
         * @since 3.2
         * @param TInt aResourceId1
         * @param TInt aResourceId2
         * @return HBufC*
         */
        IMPORT_C static HBufC* CombineStringFromResourceLC( TInt aResourceId1, TInt aResourceId2 );

        /**
         * 401-1815 Ease of Instant Messaging branding
         * Code added for the above requirment
         * This function is used to initialize the Central repository instance
         * it can be called during the init of the application. As the settings
         * are now moved to cenrep, the function IntResourceValueL will be using
         * the Cenrep instance to get the values.
         */
        IMPORT_C static void InitializeLibL();

        /**
         * 401-1815 Ease of Instant Messaging branding
         * Code added for the above requirment
         * This function is used to delete the CRepository instance, instantiated by
         * the InitializeLibL.
         */
        IMPORT_C static void UnInitializeLib();

    private:
        /* 401-1815 Ease of Instant Messaging branding
         * Code added for the above requirment
         */
        static CIMUtilsTlsData* TlsData();

    };




#endif      // IMUTILS_H

// End of File
