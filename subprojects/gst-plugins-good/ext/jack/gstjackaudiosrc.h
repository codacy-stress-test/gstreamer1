/* GStreamer
 * Copyright (C) 2008 Tristan Matthews <tristan@sat.qc.ca>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __GST_JACK_AUDIO_SRC_H__
#define __GST_JACK_AUDIO_SRC_H__

#include <gst/gst.h>
#include <gst/audio/gstaudiosrc.h>

#include "gstjackaudioclient.h"
#include "gstjack.h"
#include "gstjackloader.h"

G_BEGIN_DECLS

#define GST_TYPE_JACK_AUDIO_SRC (gst_jack_audio_src_get_type())
G_DECLARE_FINAL_TYPE (GstJackAudioSrc, gst_jack_audio_src,
    GST, JACK_AUDIO_SRC, GstAudioBaseSrc)

struct _GstJackAudioSrc
{
    GstAudioBaseSrc src;

    /*< private >*/
    /* cached caps */
    GstCaps         *caps;

    /* properties */
    GstJackConnect   connect;
    gchar           *server;
    jack_client_t   *jclient;
    gchar           *client_name;
    gchar           *port_pattern;
    guint            transport;
    gboolean         low_latency;
    gchar           *port_names;

    /* our client */
    GstJackAudioClient *client;

    /* our ports */
    jack_port_t    **ports;
    int port_count;
    sample_t **buffers;
};

G_END_DECLS

#endif /* __GST_JACK_AUDIO_SRC_H__ */
