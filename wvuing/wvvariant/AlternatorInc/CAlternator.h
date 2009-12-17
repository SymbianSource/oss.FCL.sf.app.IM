/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header of the module that is used for changing the used resources
*				 and/or bitmap files.
*
*/



#ifndef CALTERNATOR_H
#define CALTERNATOR_H

//  INCLUDES
#include <e32def.h>
#include <e32std.h>
#include <e32base.h>


// CONSTANTS
// module/brand/branded files, default path
const TInt KIdMaxLength = 16;		// brand/module id max length

// FORWARD DECLARATIONS
class CEikApplication;
class RReadStream;
class RWriteStream;

// CLASS DECLARATION

// Data struct to hold information for a branded file

/**
*  Use this interface class to implement operator branding to your module.
*  This class is created as a singleton object when loading the dll.
*  @lib alternator.lib
*  @since 1.2s
*/


class CAlternator : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CAlternator* InstanceL( const TDesC& aBrandRoot );

        /**
         *	Release instance
         */
        IMPORT_C static void Release();

        /**
        * Destructor.
        */
        virtual ~CAlternator();

    public: // New functions

        /**
        * Returns current alternative bitmap. If the path with the given
        * argument wasn't found an empty path is returned.
        * Otherwise the full path.
        * @since 1.2s
        * @param aModuleId dll's (or app) id. For example "IM".
        * @param aAlternative, id for alternative
        *                      (for example operator "Oper1" ) files.
        * @param aBitmapFileName bitmap's filename.
        * @return const TDesC& the bitmap path if there is any.
        *                      Empty if not found.
        */
        virtual const TDesC& CurrentBitmapFileL( const TDesC& aModuleId,
                                                 const TDesC& aAlternative,
                                                 const TDesC& aBitmapFileName ) = 0;


        /**
        * Switches specific resource file for a module.
        * @since 1.2s
        * @param aModuleId dll's (or app) id. For example "IM".
        * @param aAlternative id for alternative
        *                    (for example operator "Oper1" ) files.
        * @param aResFileName resource file's name.
        * @param aResOffset reference to resource offset.
        *                   Client using this module
        * must keep the resource offset until the resource is deleted.
        * @return TInt, KErrNone if successful, KErrNotFound if
        *               path wasn't valid.
        * In other error cases leaves with system error codes.
        */
        virtual TInt ChangeResourceL( const TDesC& aModuleId,
                                      const TDesC& aAlternative,
                                      const TDesC& aResFileName,
                                      TInt& aResOffset ) = 0;


        /**
        * This method can be used to find out whether the bitmap file can
        * be changed or not.
        * @since 1.2s
        * @param aModuleId dll's (or app) id. For example "IM".
        * @param aAlternative id for alternative
        *                     (for example operator "Oper1" ) files.
        * @param aBitmapFileName bitmap's filename.
        * @return TInt, returns KErrNone if the constructed path is ok.
        *               KErrNotFound if not.
        * If the return value is KErrNone then CurrentBitmapFile can be used
        * to retrieve the path.
        */
        virtual TInt ChangeBitmapFileL( const TDesC& aModuleId,
                                        const TDesC& aAlternative,
                                        const TDesC& aBitmapFileName ) = 0;


        /**
         * Constructs full path for a resource file.
         * @since 1.2s
         * @param aModuleId dll's (or app) id. For example "IM".
         * @param aAlternative id for alternative
         *                     (for example operator "Oper1" ) files.
         * @param aResFileName resource file's name.
         * @param aFullPath, this contains the full path if valid path was found.
         * Otherwise this descriptor is empty.
         */
        virtual void GetCurrentResourceFileL( const TDesC& aModuleId,
                                              const TDesC& aAlternative,
                                              const TDesC& aResFileName,
                                              TDes& aFullPath ) = 0;



    protected:

        /**
        * C++ default constructor.
        */
        CAlternator();

    };

#endif      // CALTERNATOR_H

// End of File
