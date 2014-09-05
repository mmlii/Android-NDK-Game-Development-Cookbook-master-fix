/*
 * Copyright (C) 2013 Sergey Kosarevsky (sk@linderdaum.com)
 * Copyright (C) 2013 Viktor Latypov (vl@linderdaum.com)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must display the names 'Sergey Kosarevsky' and
 *    'Viktor Latypov'in the credits of the application, if such credits exist.
 *    The authors of this work must be notified via email (sk@linderdaum.com) in
 *    this case of redistribution.
 *
 * 3. Neither the name of copyright holders nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "CurlWrap.h"
#include "Downloader.h"
#include "Blob.h"

#include <stdio.h>

bool g_ShouldExit;

class TestCallback: public DownloadCompleteCallback
{
public:
	TestCallback() {}

	virtual void Invoke()
	{
		printf( "Download complete\n" );

		printf( "Got response:\n%s\n", ( unsigned char* )FResult->GetData() );

		g_ShouldExit = true;
	}
};

int main()
{
	Curl_Load();

	g_ShouldExit = false;

	iAsyncQueue* Events = new iAsyncQueue();

	Downloader* d = new Downloader();
	d->FEventQueue = Events;

	Sleep( 1000 );

	d->DownloadURL( "http://api.flickr.com/services/rest/?method=flickr.test.echo&name=value", 1, new TestCallback() );

	while ( !g_ShouldExit )
	{
		// dispatch events here
		Events->DemultiplexEvents();
	}

	delete d;

	return 0;
}