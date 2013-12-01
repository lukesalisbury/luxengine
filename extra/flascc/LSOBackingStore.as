////////////////////////////////////////////////////////////////////////////////
//
//  ADOBE SYSTEMS INCORPORATED
//  Copyright 2012 Adobe Systems Incorporated
//  All Rights Reserved.
//
//  NOTICE: Adobe permits you to use, modify, and distribute this file
//  in accordance with the terms of the license agreement accompanying it.
//
////////////////////////////////////////////////////////////////////////////////

package com.adobe.flascc.vfs {
    import flash.net.SharedObject;

    /**
    * Extends the InMemoryBackingStore with one that stores and retrieves its data from a Flash
    * Local Shared object (<a href="http://www.adobe.com/security/flashplayer/articles/lso/" target="_new">http://www.adobe.com/security/flashplayer/articles/lso/</a>).
    * 
    * <p>The contents of this VFS will persist until the user clears their LSO cache (which might happen when they clear their browser cache.)</p>
    */
    public class LSOBackingStore extends InMemoryBackingStore {

        private var _so:SharedObject;

        /**
        * Construct an LSOBackingStore using a named local shared object.
        * @name the name of the Local Shared Object to be used, the property "filemap" will be used to store the VFS.
        */
        public function LSOBackingStore(name:String):void {
            _so = SharedObject.getLocal(name);
            if (_so && _so.data.hasOwnProperty("filemap")) {
                for(var f:String in _so.data.filemap) {
                    addFile(f, _so.data.filemap[f])
                }
            }
        }

        /**
        * Writes all of the files back into the LSO, this might be slow so you need to decide when to call
        * this in your application to balance performance with reliability.
        */
        override public function flush():void {
            super.flush();
            _so.data.filemap = {}
            for each(var f:String in getPaths()) {
                _so.data.filemap[f] = getFile(f)
            }
            _so.flush();
        }
    }

}
