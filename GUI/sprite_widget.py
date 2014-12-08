import pygtk
pygtk.require('2.0')
import gtk

def obj_mmap(obj,methods):
    """format [(f,a1,a2),...] """
    for m in methods:
        m[0](obj,*m[1:])
    return obj

class NumberEntry(gtk.Entry):
    def __init__(self):
        gtk.Entry.__init__(self)
        self.connect('changed', self.on_changed)

    def on_changed(self, *args):
        text = self.get_text().strip()
        self.set_text(''.join([text[i] for i in range(len(text)) if text[i] in '0123456789.' or (i==0 and text[i] in '-+')]))

class ImagePreview:
    def __init__(self,image):
        # Deal with the scrollable sprite view area
        self.box = gtk.VBox()

        self.previmagefile = ""
        self.imagefile = image
        self.image = None
        self.imagepreview = gtk.Image()

        table = gtk.Table(2,2)
        self.layout = gtk.Layout()
        self.layout.add(self.imagepreview)
        self.layout.connect("expose-event",self.cb_slider)

        table.attach(self.layout,0,1,0,1)
        self.hscroll = gtk.HScrollbar(self.layout.get_hadjustment())
        self.vscroll = gtk.VScrollbar(self.layout.get_vadjustment())
        table.attach(self.hscroll,0,1,1,2,gtk.FILL|gtk.SHRINK,gtk.FILL|gtk.SHRINK)
        table.attach(self.vscroll,1,2,0,1,gtk.FILL|gtk.SHRINK,gtk.FILL|gtk.SHRINK)

        self.prevzoom = 0.0
        self.zoomadj = gtk.Adjustment(1.0,0.1,10.0,0.1,1.0,1.0)
        self.slider = gtk.HScale(self.zoomadj)
        self.slider.set_update_policy(gtk.UPDATE_CONTINUOUS)
        self.slider.set_digits(1)
        self.slider.set_value_pos(gtk.POS_TOP)
        self.slider.set_draw_value(False)
        self.slider.connect("value-changed",self.cb_slider)

        self.box.pack_start(table, True, True)
        self.box.pack_start(self.slider, False, False)

    def cb_slider(self,widget, data= None):
        self.update()

    def new_image(self):
        self.previmagefile = self.imagefile
        self.image = gtk.gdk.pixbuf_new_from_file(self.imagefile)
        self.update_image()
        self.zoomadj.set_value(1.0)

    def update_image(self):
        w,h = self.image.get_width(), self.image.get_height()
        zoom = self.zoomadj.get_value()
        newimage = self.image.scale_simple(int(zoom * w), int(zoom * h), gtk.gdk.INTERP_TILES)
        self.imagepreview.set_from_pixbuf(newimage)
        self.layout.set_size(int(zoom*w),int(zoom*h))

    def update(self):
        if self.imagefile != self.previmagefile:
            self.new_image()

        if self.image:
            zoom = self.zoomadj.get_value()
            if abs(self.prevzoom - zoom) > 0.0:
                self.update_image()
            self.prevzoom = zoom

        hadj = self.hscroll.get_adjustment()
        if hadj.get_upper() - hadj.get_lower() <= hadj.get_page_size() + 0.1:
            self.hscroll.hide()
        else:
            self.hscroll.show()
        vadj = self.vscroll.get_adjustment()
        if vadj.get_upper() - vadj.get_lower() <= vadj.get_page_size() + 0.1:
            self.vscroll.hide()
        else:
            self.vscroll.show()

class Sprite_handler:
    def delete_event(self, widget, event, data=None):
        return False

    def destroy(self, widget, data= None):
        gtk.main_quit()

    def __init__(self, dotsprite=None):
        if dotsprite:
            self.f = dotsprite
            self.load()
        else:
            self.f = None
            self.config = dict()

        self.labels = dict()

        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
        self.window.resize(480,360)
        self.window.connect("delete_event", self.delete_event)
        self.window.connect("destroy", self.destroy)

        self.mainbox = gtk.HBox(spacing=6)
        self.leftbox = gtk.VBox(spacing=6)
        self.entrytable = gtk.Table(2,10)
        self.preview = ImagePreview(self.config.get('image',""))

        self.mainbox.pack_start(self.leftbox,False,False,0)
        self.mainbox.pack_start(gtk.VSeparator(),False,False,0)
        self.mainbox.pack_start(self.preview.box,True,True,0)

        self.leftbox.pack_start(self.addlabel("Sprite Properties"),False,False)
        self.leftbox.pack_start(gtk.HSeparator(),False,False)
        self.leftbox.pack_start(self.entrytable,False,False)

        # Name
        self.nameentry = gtk.Entry()
        self.nameentry.set_text("<unnamed sprite>")
        self.nameentry.connect("activate",self.cb_nameentry_changed)
        self.entrytable.attach(self.addlabel("Name"),0,1,0,1)
        self.entrytable.attach(self.nameentry,1,2,0,1)

        # File
        self.fileentry = gtk.FileChooserButton("Choose a file")
        self.fileentry.set_current_folder("/home/okke/projects/gamecreator/pikkenzuiger/")
        self.fileentry.connect("file-set",self.cb_fileentry_changed)
        self.entrytable.attach(self.addlabel('File'),0,1,1,2)
        self.entrytable.attach(self.fileentry,1,2,1,2)

        spacer = gtk.Label("")
        spacer.set_size_request(0,10)
        self.entrytable.attach(spacer,0,2,2,3)

        #frame
        self.entrytable.attach(self.addlabel('Frame Dimensions'),0,2,3,4)

        self.frame_w_entry = NumberEntry()
        self.frame_w_entry.set_text("-1")
        self.frame_w_entry.connect("activate",self.cb_dim_changed)
        self.entrytable.attach(self.addlabel("Width"),0,1,4,5)
        self.entrytable.attach(self.frame_w_entry,1,2,4,5)

        self.frame_h_entry = NumberEntry()
        self.frame_h_entry.set_text("-1")
        self.frame_h_entry.connect("activate",self.cb_dim_changed)
        self.entrytable.attach(self.addlabel("Height"),0,1,5,6)
        self.entrytable.attach(self.frame_h_entry,1,2,5,6)

        spacer = gtk.Label("")
        spacer.set_size_request(0,10)
        self.entrytable.attach(spacer,0,2,6,7)

        #Animation
        self.entrytable.attach(self.addlabel('Animation'),0,2,7,8)

        self.animdelay_entry = NumberEntry()
        self.animdelay_entry.set_text(str(int(self.config.get("animdelay",1))))
        self.animdelay_entry.connect("activate",self.cb_anim_changed)
        self.entrytable.attach(self.addlabel("Delay"),0,1,8,9)
        self.entrytable.attach(self.animdelay_entry,1,2,8,9)

        #Buttons
        self.savebutton = gtk.Button(stock=gtk.STOCK_SAVE)
        self.savebutton.connect("clicked",self.cb_save)
        self.saveasbutton = gtk.Button(stock=gtk.STOCK_SAVE_AS)
        self.saveasbutton.connect("clicked",self.cb_saveas)
        self.quitbutton = gtk.Button(stock=gtk.STOCK_QUIT)
        self.quitbutton.connect("clicked",self.destroy)
        self.leftbox.pack_end(self.quitbutton,False,False)
        self.leftbox.pack_end(self.saveasbutton,False,False)
        self.leftbox.pack_end(self.savebutton,False,False)

        self.window.add(self.mainbox)
        self.window.show_all()

        self.update()

    def activate_all(self):
        self.cb_nameentry_changed(None)
        self.cb_dim_changed(None)
        self.cb_anim_changed(None)
        self.cb_fileentry_changed(None)

    def cb_save(self, widget, data=None):
        print "saving"
        self.save()

    def cb_saveas(self, widget, data=None):
        print "saving"

    def cb_nameentry_changed(self, widget, data=None):
        self.config['name'] = self.nameentry.get_text()

    def cb_dim_changed(self, widget, data=None):
        self.config['framewidth']  = int(self.frame_w_entry.get_text())
        self.config['frameheight'] = int(self.frame_h_entry.get_text())

    def cb_anim_changed(self, widget, data=None):
        self.config['animdelay']  = int(self.animdelay_entry.get_text())

    def cb_fileentry_changed(self, widget, data=None):
        self.config['image'] = self.fileentry.get_filename()
        self.preview.imagefile = self.config['image']
        self.preview.new_image()

    def addlabel(self,key):
        label = gtk.Label(key)
        w,h = label.size_request()
        label.set_size_request(w+10,h)
        self.labels[key] = label
        return self.labels[key]

    def load(self):
        from libconfig import parseObj
        with open(self.f,'r') as f:
            self.config = parseObj(f.read())['sprite']

    def save(self):
        from libconfig import parseObj
        self.activate_all()
        configobj = parseObj("sprite : {}")
        configobj['sprite'].entries = self.config
        savestr = str(configobj)[2:-3]
        with open(self.f,"w") as f:
            f.write(savestr)

    def update(self):
        global gameroot
        if self.f:
            if 'image' in self.config:
                self.fileentry.set_filename(self.config['image'])
                self.preview.imagefile = self.config['image']
                try:
                    self.preview.update()
                except:
                    print "Image failed to load at",self.config['image']
                    pass
            if 'name' in self.config:
                self.nameentry.set_text(self.config['name'])
            if 'framewidth' in self.config:
                self.frame_w_entry.set_text(str(self.config['framewidth']))
            if 'frameheight' in self.config:
                self.frame_h_entry.set_text(str(self.config['frameheight']))
            if 'animdelay' in self.config:
                self.animdelay_entry.set_text(str(self.config['animdelay']))
    gameroot = "/home/okke/gamecreator/games/pikkenzuiger/"

if __name__ == "__main__":
    sprite = Sprite_handler('games/pikkenzuiger/sprites/kekje.sprite')
    try:
        gtk.main()
    except KeyboardInterrupt:
        gtk.main_quit()
