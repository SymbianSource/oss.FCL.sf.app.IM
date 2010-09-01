/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: this class writes the given data fragemnt to xml file.
*
*/

#ifndef __PENGPACKETLOGGER_H__
#define __PENGPACKETLOGGER_H__

//  INCLUDES
#include    "PresenceDebugPrint.h"
#include    <F32File.h>


// CONSTS
_LIT( KLogsFolder, "C:\\logs\\peng\\" ); // CSI: 25 #
_LIT( KLogsOut, "_Out_" );
_LIT( KLogsIn, "_In_" );
_LIT( KLogsNotify, "_Notf_" );
_LIT( KLogsSuffix, ".xml" );
_LIT( KUnderLine, "_" );
_LIT( KNotifFileName, "_Notf_*.xml" );
_LIT( KAllFilesMask, "*.*" );

// for logging
_LIT8( KLog_WVEngineError, "<WVEngineErrorCode>%d</WVEngineErrorCode>" );
_LIT8( KLog_PecOpCode, "<PEC><OpCode>%d</OpCode>" );
_LIT8( KLog_WVEngOpCode, "<WVEngineOpCode>%d</WVEngineOpCode>" );
_LIT8( KLog_ConnectionId, "<ConnectionId>%d</ConnectionId>" );
_LIT8( KLog_TransTime, "<TransactionTime>%d:%d:%d:%d</TransactionTime>" );
_LIT8( KLog_Tag_UserName, "<UserName>" );
_LIT8( KLog_Tag_End_UserName, "</UserName>" );
_LIT8( KLog_Tag_ServAddress, "<ServerAddress>" );
_LIT8( KLog_Tag_End_ServAddress, "</ServerAddress>" );
_LIT8( KLog_Tag_End_Pec, "</PEC>" );


const TInt KConnIdLength = 2;
const TInt KOpIdLength =  4;
const TInt KNotifyIdLength = 3;
const TInt KErrorCodeLength = 60;
const TInt KLogsFileNameLength = 50;


// CLASS DECLARATION
/**
 *  Writes the given data fragemnt to xml file.
 *
 *  @lib PEngTraAdap.lib
 *  @since 2.1
 */
class PEngPacketLogger
    {
    public:  // Logging functions

        /**
         * Writes a send packet.
         *
         * @param aData The buffer.
         * @param aOpCode The Operation id.
         *
         * @since 3.0
         * @return returns connection id, if it was gained again
         */
        inline static TInt WriteOutPacket( const TDesC8& aData,
                                           TInt aOpCode,
                                           TInt aWVEngOpCode,
                                           TInt aConnectionId,
                                           const TDesC& aUserName,
                                           const TDesC& aUrl )
            {
            return DoWrite( aData,
                            aOpCode,
                            aWVEngOpCode,
                            KLogsOut,
                            aConnectionId,
                            aUserName,
                            aUrl );
            }


        /**
         * Writes a received packet.
         *
         * @param aData The buffer.
         * @param aOpCode The Operation id.
         *
         * @since 3.0
         */
        inline static TInt WriteInPacket( const TDesC8& aData,
                                          TInt aOpCode,
                                          TInt aWVEngOpCode,
                                          TInt aConnectionId,
                                          const TDesC& aUserName,
                                          const TDesC& aUrl )
            {
            return DoWrite( aData,
                            aOpCode,
                            aWVEngOpCode,
                            KLogsIn,
                            aConnectionId,
                            aUserName,
                            aUrl );
            }


        /**
         * Writes a notification packet.
         *
         * @param aData The buffer.
         * @param aOpCode The Operation id.
         *
         * @since 3.0
         */
        inline static TInt WriteNotifyPacket( const TDesC8& aData,
                                              TInt aOpCode,
                                              TInt aConnectionId,
                                              const TDesC& aUserName,
                                              const TDesC& aUrl )
            {
            return DoWriteNotification( aData,
                                        aOpCode,
                                        KLogsNotify,
                                        aConnectionId,
                                        aUserName,
                                        aUrl );
            }

        inline static TInt WriteError( TInt aErrorCode,
                                       TInt aOpCode,
                                       TInt aWVEngOpCode,
                                       TInt aConnectionId,
                                       const TDesC& aUserName,
                                       const TDesC& aUrl )
            {
            TBuf8<KErrorCodeLength> buf;
            buf.Format( KLog_WVEngineError, aErrorCode );
            return DoWrite( buf,
                            aOpCode,
                            aWVEngOpCode,
                            KLogsIn,
                            aConnectionId,
                            aUserName,
                            aUrl );
            }

        /**
         * Get connection id
         *
         * @since 3.0
         * @return connection id
         */
        inline static TInt GetConnectionId()
            {
            return ResolveConnectionId();
            }

    private:  // Implementation

        /**
         *
         */
        inline static TInt ConnectFs( RFs& aFs )
            {
            if ( aFs.Connect() != KErrNone )
                {
                return KErrNotFound;
                }

            TEntry entry;
            if ( aFs.Entry( KLogsFolder, entry ) != KErrNone )
                {
                //directory doesn't exist
                aFs.Close();
                return KErrNotFound;
                }
            return KErrNone;
            }

        /**
         *
         */
        inline static void AppendNum( TDes& aDes, TInt aValue, TInt aDigits )
            {
            TInt marker( aValue );
            TBuf<5> value;
            for ( TInt x( 0 ) ; x < aDigits  ; ++x )
                {
                TBuf<1> oneDig;
                TInt numVal( marker % 10 );
                marker /= 10;
                oneDig.Num( numVal );
                value.Insert( 0, oneDig );
                }
            aDes.Append( value );
            }

        inline static TInt WriteToFile(
            RFs& aFs,
            const TDesC& aFileName,
            const TDesC8& aData,
            TInt aOpCode,
            TInt aWVEngOpCode,
            TInt aConnectionId,
            const TDesC& aUserName,
            const TDesC& aUrl )
            {
            PENG_DP( D_PENG_LIT( "PEC:TransAdap Log File: " ), &aFileName );
            RFile f;
            if ( f.Create( aFs, aFileName, EFileWrite ) != KErrNone )
                {
                return aConnectionId;
                }
            TBuf8<0x300> buf;
            buf.Format( KLog_PecOpCode, aOpCode );
            buf.AppendFormat( KLog_WVEngOpCode, aWVEngOpCode );
            buf.AppendFormat( KLog_ConnectionId, aConnectionId );
            // write time
            TTime time;
            time.HomeTime();
            TDateTime dayTime( time.DateTime() );
            buf.AppendFormat( KLog_TransTime,
                              dayTime.Hour(),
                              dayTime.Minute(),
                              dayTime.Second(),
                              dayTime.MicroSecond() );
            buf.Append( KLog_Tag_UserName );
            buf.Append( aUserName );
            buf.Append( KLog_Tag_End_UserName );
            buf.Append( KLog_Tag_ServAddress );
            buf.Append( aUrl );
            buf.Append( KLog_Tag_End_ServAddress );

            f.Write( buf );
            f.Write( aData );
            buf.Copy( KLog_Tag_End_Pec );
            f.Write( buf );
            f.Close();
            return aConnectionId;
            }
        /**
         * Writes packet.
         * @since 3.0
         */
        inline static TInt DoWrite( const TDesC8& aData,
                                    TInt aOpCode,
                                    TInt aWVEngOpCode,
                                    const TDesC& aFileNameBase,
                                    TInt aConnectionId,
                                    const TDesC& aUserName,
                                    const TDesC& aUrl )
            {
            RFs fs;
            TInt err( ConnectFs( fs ) );
            if ( err != KErrNone )
                {
                return err;
                }

            //locate free file name
            TInt connectionId( aConnectionId );
            if ( connectionId == KErrNotFound )
                {
                connectionId = ResolveConnectionId();
                }
            if ( connectionId < 0 )
                {
                return connectionId;
                }

            // test file name freelance
            TBuf<KLogsFileNameLength> fileName;
            fileName.Copy( KLogsFolder );
            AppendNum( fileName, connectionId, KConnIdLength  );
            fileName.Append( aFileNameBase );
            AppendNum( fileName, aOpCode, KOpIdLength );
            fileName.Append( KLogsSuffix );
            TEntry entry;
            if ( fs.Entry( fileName, entry ) == KErrNone )
                {
                fs.Delete( fileName );
                }
            err = WriteToFile( fs,
                               fileName,
                               aData,
                               aOpCode,
                               aWVEngOpCode,
                               connectionId,
                               aUserName,
                               aUrl );
            fs.Close();
            return err;
            }

        /**
         * Writes packet.
         * @since 3.0
         */
        inline static TInt DoWriteNotification( const TDesC8& aData,
                                                TInt aOpCode,
                                                const TDesC& aFileNameBase,
                                                TInt aConnectionId,
                                                const TDesC& aUserName,
                                                const TDesC& aUrl )
            {
            RFs fs;
            TInt err( ConnectFs( fs ) );
            if ( err != KErrNone )
                {
                return err;
                }

            //locate free file name
            TInt connectionId( aConnectionId );
            if ( connectionId == KErrNotFound )
                {
                connectionId = ResolveConnectionId();
                }
            if ( connectionId < 0 )
                {
                return connectionId;
                }

            //locate free file name
            CDir* dirList = NULL;
            TBuf<15> wildCard;
            AppendNum( wildCard, connectionId, KConnIdLength  );
            wildCard.Append( KNotifFileName );
            TInt counter( 0 );
            TFindFile ff( fs );
            if ( ff.FindWildByDir( wildCard, KLogsFolder, dirList ) == KErrNone )
                {
                TInt count( dirList->Count() );
                for ( TInt x ( 0 ) ; x < count ; ++x )
                    {
                    if ( ( *dirList )[x].IsDir() )
                        {
                        continue;
                        }
                    TLex check;
                    check.Assign( ( *dirList )[x].iName.Mid( 8, 3 ) );
                    TInt foundCount( KErrNotFound );
                    if ( ( KErrNone == check.Val( foundCount ) ) && ( foundCount >= counter ) )
                        {
                        counter = foundCount + 1;
                        }
                    }
                }
            delete dirList;

            TBuf<KLogsFileNameLength> fileName;
            fileName.Append( KLogsFolder );
            AppendNum( fileName, connectionId, KConnIdLength  );
            fileName.Append( aFileNameBase );
            AppendNum( fileName, counter, KNotifyIdLength );
            fileName.Append( KUnderLine );
            AppendNum( fileName, aOpCode, KOpIdLength );
            fileName.Append( KLogsSuffix );

            err = WriteToFile( fs,
                               fileName,
                               aData,
                               0,
                               0,
                               connectionId,
                               aUserName,
                               aUrl );
            fs.Close();
            return err;
            }

        inline static TInt ResolveConnectionId()
            {
            RFs fs;
            TInt err( ConnectFs( fs ) );
            if ( err != KErrNone )
                {
                return err;
                }

            //locate free connection Id
            TFindFile ff( fs );
            CDir* dirList = NULL;
            TInt connectionId( 0 );
            if ( ff.FindWildByDir( KAllFilesMask, KLogsFolder, dirList ) == KErrNone )
                {
                TInt count( dirList->Count() );
                for ( TInt x ( 0 ) ; x < count ; ++x )
                    {
                    if ( ( *dirList )[x].IsDir() )
                        {
                        continue;
                        }
                    TLex check;
                    TInt foundId( KErrNotFound );
                    TPtrC fileName( ( *dirList )[x].iName );
                    if (
                        (
                            ( KErrNotFound != fileName.FindF( KLogsIn ) )
                            &&
                            ( ( *dirList )[x].iName.Length() > 6 )
                        )
                        ||
                        (
                            ( KErrNotFound != fileName.FindF( KLogsOut ) )
                            &&
                            ( ( *dirList )[x].iName.Length() > 7 )
                        )
                        ||
                        (
                            ( KErrNotFound != fileName.FindF( KLogsNotify ) )
                            &&
                            ( ( *dirList )[x].iName.Length() > 10 )
                        )
                    )
                        {
                        check.Assign( fileName.Left( 2 ) );
                        check.Val( foundId );
                        }
                    if ( foundId >= connectionId )
                        {
                        connectionId = foundId + 1;
                        }
                    }
                fs.Close();
                delete dirList;
                return connectionId;
                }
            fs.Close();
            delete dirList;
            return connectionId;
            }
    };
#endif      // __PENGPACKETLOGGER_H__


