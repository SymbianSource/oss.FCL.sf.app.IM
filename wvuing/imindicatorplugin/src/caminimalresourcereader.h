/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Minimal resourcereader for reading resources without CONE

*                facilities

*
*/




#ifndef CAMINIMALRESOURCEREADER_H

#define CAMINIMALRESOURCEREADER_H



#include <e32base.h>

#include <barsc.h>





/**

 *  Minimal resourcereader for reading resources without CONE

 *  facilities

 *

 *  @lib ImIndicatorPlugin.dll

 *  @since S60 v3.2

 */



// This is not a C-class although it begins with a CA

class CAMinimalResourceReader   // CSI: 51 #see above

    {

    public:



        static CAMinimalResourceReader* NewL();



        static CAMinimalResourceReader* NewLC();



        virtual ~CAMinimalResourceReader();



        /**

         * Reads text from resource.

         * @since S60 v3.2

         * @param aTextResourceId ResourceID to read

         * @return Text from resource

         *         NOTE! Ownership is transferred

         */



        HBufC* ReadTextResourceL( TInt aTextResourceId );



        /**

         * Opens given resource file in current language.

         * This needs to be called before any reading!

         * @since S60 v3.2

         * @param aResourceName resource file to open

         */

        void OpenResourceFileL( TDesC& aResourceName );



        /**

         * Cleanup reader and release handles.

         * @since S60 v3.2

         */

        void Release();



    private:



        CAMinimalResourceReader();



        void ConstructL();



        /**

         * Helper method for finding resource with correct language

         * @since S60 v3.2

         * @param aName Resourcefile we should use (result)

         * @param aResourceFile source

         */

        void NearestForCurrentLanguage( TFileName& aName, const TDesC& aResourceFile );



    private: // data



        /**

         * Resourcefile

         */

        RResourceFile iResourceFile;



        /**

         * File system handle

         */

        RFs iRFs;

    };



#endif // CAMINIMALRESOURCEREADER_H

