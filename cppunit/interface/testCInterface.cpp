/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:cindent:textwidth=0:
 *
 * Copyright (C) 2005 Dell Inc.
 *  by Michael Brown <Michael_E_Brown@dell.com>
 * Licensed under the Open Software License version 2.1
 *
 * Alternatively, you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.

 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 */

// compat header should always be first header if including system headers
#include "smbios_c/compat.h"

#include <iomanip>
#include <fstream>

#include "testCInterface.h"
#include "smbios_c/memory.h"
#include "smbios_c/version.h"

#include "outputctl.h"

using namespace std;

// Note:
//      Except for , there are no "using namespace XXXX;" statements
//      here... on purpose. We want to ensure that while reading this code that
//      it is extremely obvious where each function is coming from.
//
//      This leads to verbose code in some instances, but that is fine for
//      these purposes.

// Register the test
CPPUNIT_TEST_SUITE_REGISTRATION (testCInterface);

void copyFile( string dstFile, string srcFile )
{
    ifstream src(srcFile.c_str(), ios_base::binary);
    ofstream dst(dstFile.c_str(), ios_base::out | ios_base::binary | ios_base::trunc);

    char ch;
    while( src.get(ch)) dst.put(ch);

    if( !src.eof() || !dst ) throw exception();
}

bool fileExists(string fileName)
{
    FILE *fh=0;
    fh=fopen(fileName.c_str(), "rb");
    if(!fh)
        return false;

    fclose(fh);
    return true;
}

void testCInterface::setUp()
{
    string writeDirectory = getWritableDirectory();
    string testFile = writeDirectory + "/testmem.dat";

    FILE *fd = fopen(testFile.c_str(), "w+");
    for (int i=0; i<26; i++)
    {
        char w = 'a' + i;
        fwrite(&w, 1, 1, fd);  // void *ptr, size, nmemb, FILE *
    }
    fflush(fd);
    fclose(fd);

    memory_factory(MEMORY_UNIT_TEST_MODE | MEMORY_GET_SINGLETON, testFile.c_str());
}

void testCInterface::tearDown()
{ }

void testCInterface::testMemoryRead()
{
    STD_TEST_START(getTestName().c_str() << "  ");

    u8 buf;
    struct memory *m = 0;
    int ret;
    for (int i=0; i<6; ++i){
        m = memory_factory(MEMORY_GET_SINGLETON);
        buf = '9';
        ret = memory_read(m, &buf, i, 1);
        CPPUNIT_ASSERT_EQUAL( 0, ret );
        CPPUNIT_ASSERT_EQUAL( buf, (u8)('a' + i) );
        memory_free(m);
    }

    STD_TEST_END("");
}


void testCInterface::testMemoryWrite()
{
    STD_TEST_START(getTestName().c_str() << "  ");
    struct memory *m = memory_factory(MEMORY_GET_SINGLETON);
    u8 buf;
    int ret;

    for (int i=0; i<26; ++i){
        ret = memory_read(m, &buf, i, 1);
        CPPUNIT_ASSERT_EQUAL( 0, ret );
        CPPUNIT_ASSERT_EQUAL( buf, (u8)('a' + i) );
    }

    for (int i=0; i<26; ++i){
        ret = memory_read(m, &buf, i, 1);
        CPPUNIT_ASSERT_EQUAL( 0, ret );
        buf = buf + 'A' - 'a';
        ret = memory_write(m, &buf, i, 1);
        CPPUNIT_ASSERT_EQUAL( 0, ret );
    }

    for (int i=0; i<26; ++i){
        ret = memory_read(m, &buf, i, 1);
        CPPUNIT_ASSERT_EQUAL( 0, ret );
        CPPUNIT_ASSERT_EQUAL( buf, (u8)('A' + i) );
    }

    memory_free(m);

    STD_TEST_END("");
}

