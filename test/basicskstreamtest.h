// basic_socket_stream test case
// Copyright (C) 2002 Dan Tomalesky and The WorldForge Project
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//  For information about Worldforge and its authors, please contact
//  the Worldforge Web Site at http://www.wordforge.org.
//
//  Author: Dan Tomalesky
//  Created: 2002-02-19 by Dan Tomalesky
//
// $Log$
// Revision 1.8  2005-01-14 17:36:36  alriddoch
// 2004-01-14  Al Riddoch  <alriddoch@zepler.org>
//
// 	* test/basicskstreamtest.h: Socket buffers are now automatically
// 	  deleted by the socket streams which own them.
//
// Revision 1.7  2003/05/06 21:53:11  alriddoch
//  2003-05-06 Al Riddoch <alriddoch@zepler.org>
//     - skstream/skstream.h, skstream/skstream.cpp, skstream_unix.h:
//       Re-work basic_socket_stream so it can have either stream or datagram
//       buffers.
//     - ping/ping.cpp, ping/ping.h, test/basicskstreamtest.h,
//       test/childskstreamtest.h, test/skservertest.h: Get the tests and examples
//       building again.
//
// Revision 1.6  2003/05/04 21:30:16  alriddoch
//  2003-05-04 Al Riddoch <alriddoch@zepler.org>
//     - Sort out nasty hard tabs from address checks.
//     - Start work on updating the checks.
//
// Revision 1.5  2002/06/12 20:48:40  xmp
// The usual std namespace stuff.  IE this was made on gcc-2.95, and gcc-3.1 is stricter.
//
// Revision 1.4  2002/03/01 14:00:09  grimicus
// 03/01/2002 Dan Tomalesky <grim@xynesis.com>
//     * Changed can_accept's timeval to be all 0, so that it is just a quick
//       poll and returns immediately (Thanks to alriddoch for the suggestion)
//
//     * Modified the ugly switch block I was using to print out error message
//       names to use strerror (Thanks to James for that one)
//
// Revision 1.3  2002/02/26 20:33:55  grimicus
// 02/26/2002 Dan Tomalesky <grim@xynesis.com>
//     * Added test cases for skserver and friends
//
//     * Adding .cvsignore files so that it doesn't mess with non-cvs files anymore
//
//     * In skserver.cpp and skstream.cpp, in the close() methods, I commented out
//       the return; in the error part of the shutdown() call because it is
//       possible that a socket can exist without it actually having been used,
//       so this could error out on those cases and the socket is never actually
//       closed.
//
//     * In skserver.h, added can_accept() to tcp_socket_server so that it can be
//       checked to see if the server socket has any connections available, so that
//       accept() can then be called. (if it returns false, if accept is called,
//       it will block until a connection is made)
//
//     * Removed the #include <iostream> from skserver.h and skstream.h as they are
//       not actually needed for any of the code. (else it comes in from some other
//       include, I'm not positive)
//
//     * Made some formatting changes in skserver.h along the same lines as I have
//       been doing throughout the code.
//
//     * Added testClose() to basicskstreamtest.
//
//     * Changed the socket created in basicskstreamtest from SOCK_STREAM to
//       SOCK_DGRAM though it doesn't make any difference what so ever in the
//       testing.
//
//     * Added the skservertests into the test runner.
//
// Revision 1.2  2002/02/24 03:15:41  grimicus
// 02/23/2002 Dan Tomalesky <grim@xynesis.com>
//
//     * Added in CVS logging variable so that changes show up in modified files
//       This will help keep changes made by worldforge in each file that is
//       changed as required by the GPL.
//
//     * Changed some parameter variables to have better meaning.
//       (ad -> address, etc.)
//
//     * Added some code into tcp_sk_stream::open so that it calls setLastError()
//       when the connection fails.
//
//     * Added some comments into skstream.h to better describe SOCKET_TYPE as
//       there can be some confusion between what it is actually for
//       (pointer/file descriptor/windows cludge of the socket) and the various
//       types of sockets (tcp, udp, raw, etc)
//
//     * Changed some more formatting for readability.
//
//     * Uncommented some commented out code in skstream.h so that the sync()
//       method returns 0 on the else, rather than not returning anything.
//
//     * Added some code into setBroadcast() so that setLastError() is called
//       if it fails to perform the getsocketopt().
//
//     * Modified the test/Makefile.am to remove the header files from the SOURCES
//       as the .h files do not seem to affect the build.
//
//     * Updated all the current test so that they use a socket instead of the
//       absolutely wrong stuff I was doing before.
//
//     * Added tests for tcp, udp, and raw skstreams child classes.
//
// Revision 1.1  2002/02/21 05:11:15  grimicus
// 2002-02-20 Dan Tomalesky <grim@xynesis.com>
//     * Added a new test case header for basic_socket_streams
//
//     * Made a mod to basic_socket_stream::setOutpeer(sockaddr_in&)
//       seemed to me it had an erroneous ! in the return
//
//     * Oh, and I reformatted skstream.h so its not so damn hard to look at
//
//

#ifndef BASICSKSTREAMTEST_H
#define BASICSKSTREAMTEST_H

#include <skstream/skstream.h>

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>
#include <iostream>

class basicskstreamtest : public CppUnit::TestCase
{
    //some macros for building the suite() method
    CPPUNIT_TEST_SUITE(basicskstreamtest);
    CPPUNIT_TEST(testConstructor_1);
    CPPUNIT_TEST(testConstructor_2);
    CPPUNIT_TEST(testConstructor_3);
    CPPUNIT_TEST(testConstructor_4);
    CPPUNIT_TEST(testSetSocket);
    CPPUNIT_TEST(testSetBroadcast);
    CPPUNIT_TEST(testClose);
    CPPUNIT_TEST_SUITE_END();

    private: 
        basic_socket_stream *skstream;
        std::string *hostname;
        SOCKET_TYPE socket;
        int port;

    public:
        basicskstreamtest(std::string name) : TestCase(name) { }
        basicskstreamtest() { }

        void testConstructor_1()
        {
            stream_socketbuf *skb = new stream_socketbuf(INVALID_SOCKET);
            basic_socket_stream *sks = new basic_socket_stream(*skb);

            CPPUNIT_ASSERT(sks);

            delete sks;
        }

        void testConstructor_2()
        {
            stream_socketbuf *skb;
            basic_socket_stream *sks;
            unsigned insize, outsize;

            for(int i = FreeSockets::proto_IP; i <= FreeSockets::proto_RAW; ++i)
            {
                //maybe make them random? :-)
                insize = i * 3;
                outsize = i * 2;

                skb = new stream_socketbuf(INVALID_SOCKET, insize, outsize);
                sks = new basic_socket_stream(*skb, i);

                CPPUNIT_ASSERT(sks);
                CPPUNIT_ASSERT(sks->getProtocol() == i);

                delete sks;
            }
            
            //test with crumby settings...should this fail?
            skb = new stream_socketbuf(INVALID_SOCKET, 0, 0);
            sks = new basic_socket_stream(*skb, (unsigned)0);
            CPPUNIT_ASSERT(sks);
            delete sks;
        }

        void testConstructor_3()
        {
            CPPUNIT_ASSERT(skstream);
        }

        void testConstructor_4()
        {
            unsigned insize = 5, outsize = 6;
            SOCKET_TYPE sock = skstream->getSocket();

            stream_socketbuf * skb = new stream_socketbuf (sock, insize, outsize);
            basic_socket_stream sks(*skb);

            CPPUNIT_ASSERT(sks);

            CPPUNIT_ASSERT(sks.getSocket() == sock);
        }

        void testSetSocket()
        {
            CPPUNIT_ASSERT(skstream->getSocket() == socket);
            CPPUNIT_ASSERT(skstream->is_open());
        }

        void testSetBroadcast()
        {
            if(!skstream->setBroadcast(true))
            {
                std::cout << std::endl;
                std::cout << "Error with broadcast: " << 
                    strerror(skstream->getLastError()) << std::endl;

                CPPUNIT_ASSERT(false);
            }
            
            CPPUNIT_ASSERT(skstream->setBroadcast(false));
        }

        void testClose()
        {
            skstream->close();

            /*
            if(skstream->is_open())
            {
                std::cout << endl << "Last Error: " << 
                     strerror(skstream->getLastError()) << std::endl;
            }
            */

            CPPUNIT_ASSERT(!skstream->is_open());
        }

        void setUp()
        {
            socket = ::socket(AF_INET, SOCK_DGRAM, FreeSockets::proto_UDP);
            stream_socketbuf * skb = new stream_socketbuf(socket);
            skstream = new basic_socket_stream(*skb);
            
            //echo service must be running (check inetd settings or if you
            //are a winders user, you have to install it)
            hostname = new std::string("127.0.0.1");
                //new std::string("localhost");
            port = 7;
        }

        void tearDown()
        {
            delete skstream;
            delete hostname;
            port = 0;
            socket = INVALID_SOCKET;
        }

};

#endif
