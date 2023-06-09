---
title: Tagging (Metadata and Streaminfo)
...

# Tagging (Metadata and Streaminfo)

## Overview

Tags are pieces of information stored in a stream that are not the
content itself, but they rather *describe* the content. Most media
container formats support tagging in one way or another. Ogg uses
VorbisComment for this, MP3 uses ID3, AVI and WAV use RIFF's INFO list
chunk, etc. GStreamer provides a general way for elements to read tags
from the stream and expose this to the user. The tags (at least the
metadata) will be part of the stream inside the pipeline. The
consequence of this is that transcoding of files from one format to
another will automatically preserve tags, as long as the input and
output format elements both support tagging.

Tags are separated in two categories in GStreamer, even though
applications won't notice anything of this. The first are called
*metadata*, the second are called *streaminfo*. Metadata are tags that
describe the non-technical parts of stream content. They can be changed
without needing to re-encode the stream completely. Examples are
“author”, “title” or “album”. The container format might still need
to be re-written for the tags to fit in, though. Streaminfo, on the
other hand, are tags that describe the stream contents technically. To
change them, the stream needs to be re-encoded. Examples are “codec” or
“bitrate”. Note that some container formats (like ID3) store various
streaminfo tags as metadata in the file container, which means that they
can be changed so that they don't match the content in the file any
more. Still, they are called metadata because *technically*, they can be
changed without re-encoding the whole stream, even though that makes
them invalid. Files with such metadata tags will have the same tag
twice: once as metadata, once as streaminfo.

There is no special name for tag reading elements in GStreamer. There
are specialised elements (e.g. id3demux) that do nothing besides tag
reading, but any GStreamer element may extract tags while processing
data, and most decoders, demuxers and parsers do.

A tag writer is called
[`TagSetter`](GstTagSetter). An element
supporting both can be used in a tag editor for quick tag changing
(note: in-place tag editing is still poorly supported at the time of
writing and usually requires tag extraction/stripping and remuxing of
the stream with new tags).

## Reading Tags from Streams

The basic object for tags is a [`GstTagList`](GstTagList). An element that is reading
tags from a stream should create an empty taglist and fill this with
individual tags. Empty tag lists can be created with `gst_tag_list_new
()`. Then, the element can fill the list using `gst_tag_list_add ()
` or `gst_tag_list_add_values ()`. Note that elements often read
metadata as strings, but the values in the taglist might not necessarily
be strings - they need to be of the type the tag was registered as (the
API documentation for each predefined tag should contain the type). Be
sure to use functions like `gst_value_transform ()` to make sure that
your data is of the right type. After data reading, you can send the
tags downstream with the TAG event. When the TAG event reaches the sink,
it will post the TAG message on the pipeline's GstBus for the
application to pick up.

We currently require the core to know the GType of tags before they are
being used, so all tags must be registered first. You can add new tags
to the list of known tags using `gst_tag_register ()`. If you think the
tag will be useful in more cases than just your own element, it might be
a good idea to add it to `gsttag.c` instead. That's up to you to decide.
If you want to do it in your own element, it's easiest to register the
tag in one of your class init functions, preferably `_class_init ()`.

``` c

static void
gst_my_filter_class_init (GstMyFilterClass *klass)
{
[..]
  gst_tag_register ("my_tag_name", GST_TAG_FLAG_META,
            G_TYPE_STRING,
            _("my own tag"),
            _("a tag that is specific to my own element"),
            NULL);
[..]
}


```

## Writing Tags to Streams

Tag writers are the opposite of tag readers. Tag writers only take
metadata tags into account, since that's the only type of tags that have
to be written into a stream. Tag writers can receive tags in three ways:
internal, application and pipeline. Internal tags are tags read by the
element itself, which means that the tag writer is - in that case - a
tag reader, too. Application tags are tags provided to the element via
the TagSetter interface (which is just a layer). Pipeline tags are tags
provided to the element from within the pipeline. The element receives
such tags via the `GST_EVENT_TAG` event, which means that tags writers
should implement an event handler. The tag writer is responsible for
combining all these three into one list and writing them to the output
stream.

The example below will receive tags from both application and pipeline,
combine them and write them to the output stream. It implements the tag
setter so applications can set tags, and retrieves pipeline tags from
incoming events.

Warning, this example is outdated and doesn't work with the 1.0 version
of GStreamer anymore.

``` c

GType
gst_my_filter_get_type (void)
{
[..]
    static const GInterfaceInfo tag_setter_info = {
      NULL,
      NULL,
      NULL
    };
[..]
    g_type_add_interface_static (my_filter_type,
                 GST_TYPE_TAG_SETTER,
                 &tag_setter_info);
[..]
}

static void
gst_my_filter_init (GstMyFilter *filter)
{
[..]
}

/*
 * Write one tag.
 */

static void
gst_my_filter_write_tag (const GstTagList *taglist,
             const gchar      *tagname,
             gpointer          data)
{
  GstMyFilter *filter = GST_MY_FILTER (data);
  GstBuffer *buffer;
  guint num_values = gst_tag_list_get_tag_size (list, tag_name), n;
  const GValue *from;
  GValue to = { 0 };

  g_value_init (&to, G_TYPE_STRING);

  for (n = 0; n < num_values; n++) {
    guint8 * data;
    gsize size;

    from = gst_tag_list_get_value_index (taglist, tagname, n);
    g_value_transform (from, &to);

    data = g_strdup_printf ("%s:%s", tagname,
        g_value_get_string (&to));
    size = strlen (data);

    buf = gst_buffer_new_wrapped (data, size);
    gst_pad_push (filter->srcpad, buf);
  }

  g_value_unset (&to);
}

static void
gst_my_filter_task_func (GstElement *element)
{
  GstMyFilter *filter = GST_MY_FILTER (element);
  GstTagSetter *tagsetter = GST_TAG_SETTER (element);
  GstData *data;
  GstEvent *event;
  gboolean eos = FALSE;
  GstTagList *taglist = gst_tag_list_new ();

  while (!eos) {
    data = gst_pad_pull (filter->sinkpad);

    /* We're not very much interested in data right now */
    if (GST_IS_BUFFER (data))
      gst_buffer_unref (GST_BUFFER (data));
    event = GST_EVENT (data);

    switch (GST_EVENT_TYPE (event)) {
      case GST_EVENT_TAG:
        gst_tag_list_insert (taglist, gst_event_tag_get_list (event),
                 GST_TAG_MERGE_PREPEND);
        gst_event_unref (event);
        break;
      case GST_EVENT_EOS:
        eos = TRUE;
        gst_event_unref (event);
        break;
      default:
        gst_pad_event_default (filter->sinkpad, event);
        break;
    }
  }

  /* merge tags with the ones retrieved from the application */
  if ((gst_tag_setter_get_tag_list (tagsetter)) {
    gst_tag_list_insert (taglist,
             gst_tag_setter_get_tag_list (tagsetter),
             gst_tag_setter_get_tag_merge_mode (tagsetter));
  }

  /* write tags */
  gst_tag_list_foreach (taglist, gst_my_filter_write_tag, filter);

  /* signal EOS */
  gst_pad_push (filter->srcpad, gst_event_new (GST_EVENT_EOS));
}


```

Note that normally elements would not read the full stream before
processing tags. Rather, they would read from each sinkpad until they've
received data (since tags usually come in before the first data buffer)
and process that.
