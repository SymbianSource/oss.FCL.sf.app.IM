rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description: Create settings for test cases
rem

@echo off

echo Cleaning old SAPSettings

cd \
cd \epoc32\winscw\c\private\10202be9\persists
del 101f884c.cre /S /Q

echo copying cre file

xcopy /R /Y \s60\app\imppapplications\tsrc\init\101f884c.cre 

echo Cleaning old phonebook database file
cd \
cd \epoc32\WINSCW\C\private\10003a73
del contacts.cdb /S /Q

echo Copying new phonebook database file
xcopy /R /Y \s60\app\imppapplications\tsrc\init\contacts.cdb 

cd \

echo settings created for test cases