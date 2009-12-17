/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Content writer for the XML Serializer.
*
*/

#ifndef __TPENGWRITER_H__
#define __TPENGWRITER_H__

//  INCLUDES
#include <e32base.h>



// CLASS DECLARATION
/**
 * Content writer for the XML Serializer.
 *
 * Simple class encapsulating the overflow
 * safe handling of the write buffer.
 *
 * @lib PEngParser.lib
 * @since 2.1
 */
NONSHARABLE_CLASS( TPEngWriter )
    {

public:  // Constructor and destructor

    /**
     * C++ constructor.
     * @param aBuffer Reference to buffer where
     * to write.
     */
    TPEngWriter( TDes8& aBuffer );


public:  //New buffer handling methods


    /**
     * Writes text to destination
     * buffer. Leaves with KErrOverflow
     * if the passed aText doesn't fit in the
     * buffer. Returns TPtrC8 pointing to text
     * just wrote to the buffer.
     * (Returned pointer descriptor stays valid
     * untill the text is reversed beoynd the
     * start position of current write operation.)
     *
     * @since 2.1
     * @param aText Text to write.
     * @return Pointer descriptor pointing to
     * just written text. (Pointer descriptor
     * stays valid until the buffer is reversed
     * beyond the text's start position.)
     */
    const TPtrC8 WriteL( const TDesC8& aText );


    /**
     * Writes a character to destination
     * buffer. Leaves with KErrOverflow
     * if the character doesn't fit in the
     * buffer.
     *
     * @since 2.1
     * @param aCharacter Character to write.
     */
    void WriteL( TUint8 aCharacter );



    /**
     * Gets the write buffers current length.
     *
     * @since 2.1
     * @return Write buffers current length.
     */
    TInt CurrentLength();


    /**
     * Reverses the write buffer to required
     * length.
     *
     * If required length aLength is greater
     * than current buffer length,
     * returns KErrUnderflow. Else returns KErrNone.
     *
     * @since 2.1
     * @param aLength Length to reverse to.
     * @return KErrNone or KErrUnderflow.
     */
    TInt ReverseTo( TInt aLength );



protected: // Data

    ///<Reference to buffer to write to.
    TDes8&      iBuffer;
    };

#endif      //  __TPENGWRITER_H__

//  End of File
