import pygtk
import gobject
import gtk
import gtk.gdk as gdk

def lists_eq(l1,l2):
    if len(l1) != len(l2):
        return False
    for i in range(len(l1)):
        if not l1[i] == l2[i]:
            return False
    return True

class MessageBox(gtk.ScrolledWindow):
    def __init__(self,editor,text):
        gtk.ScrolledWindow.__init__(self)
        self.set_policy(gtk.POLICY_NEVER, gtk.POLICY_AUTOMATIC)

        self.label = gtk.Label(text)
        self.label.set_alignment(0,1.0)
        self.add_with_viewport(self.label)

        self.connect("button-press-event",self.clicked)
        self.set_events(self.get_events() | gdk.BUTTON_PRESS_MASK)

        self.editor = editor

    def clicked(self,event,data=None):
        toggle = not self.expanded
        self.set_expand(toggle)

    def set_text(self,text):
        self.label.set_text(text)
        
    def set_expand(self,toggle):
        self.expanded = toggle
        if toggle:
            self.label.set_text('\n'.join(self.editor.messages))
            self.parent.set_child_packing(self,True,True,0,gtk.PACK_END)
        else:
            self.label.set_text(self.editor.messages[-1])
            w, h = self.label.size_request()
            self.set_size_request(w,h+5)
            self.parent.set_child_packing(self,False,False,0,gtk.PACK_END)

class RealLabel(gtk.EventBox):
    def __init__(self,text):
        gtk.EventBox.__init__(self)
        self.label = gtk.Label(text)
        self.add(self.label)
        
    def __getattr__(self,key):
        if key == 'size':
            return self.label.size_request()
    
    def __setattr__(self,key,val):
        if key == 'size' and isinstance(val,tuple) and len(val) == 2:
            self.label.set_size_request(*val)
        else:
            self.__dict__[key] = val
            
class LevelInfo(gtk.VBox):
    def __init__(self, editor):
        gtk.VBox.__init__(self)
        self.editor = editor

        label = RealLabel("Level Info")
        w,h = label.size
        label.size = (w+130,h+6)
        #label.modify_bg(gtk.STATE_NORMAL, gdk.color_parse("dark gray"))
        self.pack_start(label,False,False)
        self.pack_start(gtk.HSeparator(),False,False)
        self.pack_start(gtk.Label("Objects"),False,False)

        sw = gtk.ScrolledWindow()
        sw.set_shadow_type(gtk.SHADOW_ETCHED_IN)
        sw.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
        self.pack_start(sw)

        # OBJECTS
        self.objs = gtk.ListStore(gdk.Pixbuf,str)
        for i,obj in enumerate(self.editor.objs.values()):
            self.objs.append((obj.imagepreview,obj.name))
        treeview = gtk.TreeView(self.objs)
        treeview.connect("cursor-changed",self.activate)

        rendererSprite = gtk.CellRendererPixbuf()
        column = gtk.TreeViewColumn(None, rendererSprite, pixbuf=0)
        treeview.append_column(column)

        rendererText = gtk.CellRendererText()
        column = gtk.TreeViewColumn(None, rendererText, text=1)
        treeview.append_column(column)
        sw.add(treeview)
        
        self.savebutt = gtk.Button(stock=gtk.STOCK_SAVE)
        self.savebutt.connect("clicked",self.save)
        self.pack_end(self.savebutt,False,False)

        self.runbutt = gtk.Button("Save & Run")
        self.runbutt.connect("clicked",self.run)
        self.pack_end(self.runbutt,False,False)

        self.selectedobj = None

    def save(self,widget,data=None):
        self.editor.save()

    def update(self):
        pass

    def run(self,widget,data=None):
        from subprocess import Popen
        self.save(None)
        Popen(['./engine'])

    def activate(self, view):
        selection = view.get_selection()
        model, treeiter = selection.get_selected()
        self.selectedobj = self.editor.objs[model.get_value(treeiter, 1)]

class LevelPreview(gtk.VBox):
    def __init__(self,editor):
        gtk.VBox.__init__(self)
        self.editor = editor
        
        level = self.editor.level

        self.area = gtk.DrawingArea()
        try:
            self.levelsize = (level.width, level.height)
        except KeyError:
            print "failed"
            self.levelsize = (100,100)
        self.pack_start(self.area)
        self.area.connect("expose-event",self.render)
        self.area.connect("motion_notify_event",self.moved)
        self.area.connect("button-press-event",self.clicked)
        self.area.set_events(gtk.gdk.POINTER_MOTION_MASK |
                             gtk.gdk.POINTER_MOTION_HINT_MASK |
                             gtk.gdk.BUTTON_PRESS_MASK)

        from logmap import AdvancedLogMap
        self.grid = AdvancedLogMap(self.levelsize[0],self.levelsize[1])
        self.active_instance = []

        self.pack_start(gtk.HSeparator(),False,False)

        self.infobox = MessageBox(editor,"")      
        #self.infobox.set_alignment(0,0.5)
        self.infobox.set_text("")
        self.pack_start(self.infobox,False,False)
        self.infobox.set_expand(False)

        self.draw_optim = []
    
    def update(self, dest=True):
        self.render(None,dest)

    def reset_text(self):
        self.infobox.set_expand(self.infobox.expanded)

    def add_instance(self,objname,x,y):
        ins = (objname,x,y)
        self.editor.level.instances.append(ins)
        self.editor.messages.append("Added instance %s at position %.1f,%.1f" % ins)
        self.draw_optim.append(ins)

    def render(self,widget,dest):
        gc = self.area.window.new_gc()
        w, h = self.levelsize
        
        if dest:
            gc.set_rgb_fg_color(self.editor.level.gdkcolor)
            print w,h
            self.area.window.draw_rectangle(gc,True,0,0,w,h)
            
            from logmap import AdvancedLogMap
            self.grid = AdvancedLogMap(self.levelsize[0],self.levelsize[1])
            self.draw_optim = self.editor.level.instances

        for i in range(len(self.draw_optim)):
            ins = self.draw_optim[i]
            obj = self.editor.objs[ins[0]]
            obj.draw(gc, self.area.window, ins[1],ins[2])
            ow, oh = obj.w, obj.h
            self.grid.add(ins,(ins[1],ins[2],float(ow),float(oh)))
        self.draw_optim = []

        if dest:
            gc.set_rgb_fg_color(gdk.color_parse("#FF0000"))
            self.area.window.draw_rectangle(gc,False,0,0,w, h)
        
        for ins in self.active_instance:
            gc.set_rgb_fg_color(gdk.color_parse("#FF0000"))
            spr = self.editor.objs[ins[0]].spritefile
            w,h = spr.framesize()
            self.area.window.draw_rectangle(gc,False,int(ins[1]),int(ins[2]), w,h)

    def moved(self,widget,event):
        if event.is_hint:
                x, y, state = event.window.get_pointer()
        else:
            x = event.x
            y = event.y
            state = event.state

        instances = map(lambda x: x.value,self.grid.get_pos(x,y))
        prev_instance = self.active_instance
        self.active_instance = instances
        if not lists_eq(prev_instance,instances):
            self.render(None,True)

        for ins in instances:
            spr = self.editor.objs[ins[0]].spritefile
            sw, sh = spr.framesize()

            if x < ins[1] or x > ins[1] + sw or y < ins[2] or y > ins[2] + sh:
                ins = None
                self.reset_text()
            else:
                self.infobox.set_text("%s at  %i, %i" % (ins[0],int(ins[1]),int(ins[2])))

        return True

    def clicked(self,widget,event):
        x = event.x
        y = event.y
        state = event.state

        if self.active_instance != []:  
            self.editor.objectinfo.set_objs(self.active_instance)
        elif x < self.levelsize[0] and y < self.levelsize[1]:
            self.editor.objectinfo.selected = None
            obj = self.editor.levelinfo.selectedobj
            if obj:
                self.add_instance(obj.name,x,y)
                self.update(False)

        self.editor.objectinfo.update()
        return True

class Infobox(gtk.Frame):
    def __init__(self, ins, objinfo):
        gtk.Frame.__init__(self)
        self.set_border_width(4)

        box = gtk.HBox()
        box.set_border_width(2)
        self.add(box)

        self.img_w = 50
        self.img_h = 50
        self.image = gtk.image_new_from_pixbuf(new_pixbuf(self.img_w,self.img_h))
        
        self.ins = ins

        evbox = gtk.EventBox()
        evbox.add(self.image)
        evbox.set_border_width(2)
        box.pack_start(evbox)

        props = gtk.VBox()
        self.Lname = gtk.Label("Name: %s" % ins[0])
        self.Lname.set_alignment(0,0.5)
        self.Lpos = gtk.Label("Position: %.1f, %.1f" % (ins[1],ins[2]))
        self.Lpos.set_alignment(0,0.5)

        props.pack_start(self.Lname,False,False,6)
        props.pack_start(self.Lpos,False,False)

        box.pack_start(props,False,False)

        self.connect("button-press-event", self.clicked, objinfo)
        self.set_events(self.get_events() | gdk.BUTTON_PRESS_MASK)
        self.show_all()

    def set_image(self, pixbuf):
        w,h = pixbuf.get_width(), pixbuf.get_height()
        pixbuf.scale(self.image.get_pixbuf(), 0, 0, self.img_w, self.img_h, 0, 0, self.img_w/float(w) , self.img_h/float(h), gdk.INTERP_BILINEAR)
        self.image.queue_draw()

    def clicked(self, widget, data, objinfo):
        i = objinfo.editor.level.instances.index(self.ins)
        del objinfo.editor.level.instances[i]
        objinfo.editor.levelpreview.update()
        objinfo.editor.messages.append("Removed instance %s at position %.1f,%.1f" % tuple(self.ins))

        i = objinfo.selected.index(self.ins)
        del objinfo.selected[i]
        objinfo.infoboxes.remove(self)
        return True

class ObjectInfo(gtk.VBox):
    def set_objs(self, ls):
        self.selected = []
        for child in self.infoboxes.get_children():
            self.infoboxes.remove(child)
        
        for ins in ls:
            print ins
            obj = self.editor.objs[ins[0]]
            self.selected.append(ins)
            
            info = Infobox(ins, self)
            info.set_image(obj.imagepreview)
            self.infoboxes.pack_start(info,False,False)

    def __init__(self,editor):
        gtk.VBox.__init__(self)
        self.editor = editor
        
        label = RealLabel("Object Info")
        w,h = label.size
        label.size = (w+160,h+8)
        #label.modify_bg(gtk.STATE_NORMAL, gdk.color_parse("dark gray"))
        self.pack_start(label,False,False)
        self.pack_start(gtk.HSeparator(),False,False)

        self.infoboxes = gtk.VBox()
        self.pack_start(self.infoboxes)

        self.selected = []

        rembutton = gtk.Button(stock=gtk.STOCK_REMOVE)
        rembutton.connect("clicked",self.remove_selected)
        self.pack_end(rembutton,False,False)

        self.update()

    def update(self):
        if self.selected:
            self.infoboxes.show()
        else:
            self.infoboxes.hide()

    def remove_selected(self,widget,data=None):
        for ins in self.selected:
            i = self.editor.level.instances.index(ins)
            del self.editor.level.instances[i]
            self.editor.levelpreview.update()
            self.editor.messages.append("Removed instance %s at position %.1f,%.1f" % tuple(ins))

            self.selected = None
            self.update()


class LevelEditor:
    def delete_event(self, widget, event, data=None):
        return False

    def destroy(self, widget, data= None):
        gtk.main_quit()

    def load_resources(self):
        import os,glob
        gameroot = "/home/okke/projects/gamecreator/games/pikkenzuiger"

        path = os.path.join(gameroot,"objects","*.object")
        obj_files = glob.glob(path)
        self.objs = dict()
        for f in obj_files:
            obj = Object(f)
            self.objs[obj.config['name']] = obj

    def update(self):
        self.levelinfo.update()
        self.levelpreview.update()
        self.objectinfo.update()

    def __init__(self, dotlevel):
        self.load_resources()
        self.level = Level(dotlevel)
        self.messages = ["Editing level: %s" % self.level.name]
        
        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
        self.window.resize(900,500)
        self.window.connect("delete_event", self.delete_event)
        self.window.connect("destroy", self.destroy)

        self.levelinfo = LevelInfo(self)
        self.levelpreview = LevelPreview(self)
        self.objectinfo = ObjectInfo(self)

        self.box = gtk.HBox()
        self.box.pack_start(self.levelinfo,False,False)
        self.box.pack_start(gtk.VSeparator(),False,False)
        self.box.pack_start(self.levelpreview,True,True)
        self.box.pack_start(gtk.VSeparator(),False,False)
        self.box.pack_start(self.objectinfo,False,False)

        self.window.add(self.box)
        self.window.show_all()
        self.objectinfo.update()

    def save(self):
        self.level.save()

class Resource:
    def __init__(self,dotfile):
        self.config = dict()
        self.f = dotfile
        self.rtype = 'resource'
        self.load()

    def __getattr__(self,key):
        return self.config[key]

    def __setattr__(self,key,val):
        if hasattr(self,'config') and key in self.config:
            self.config[key] = val
        else:
            self.__dict__[key] = val

    def __nonzero__(self):
        return True

    def load(self):
        from resource_parse import parseObj
        with open(self.f) as f:
            config = parseObj('{' + f.read() + '}').get(self.rtype,None)
        if not hasattr(self,'config'):
            self.config = dict()
        for key in config:
            self.config[key] = config[key]
            

    def save(self):
        from resource_parse import parseObj, dict_for_file
        conf = parseObj("{"+self.rtype + " : {}}")
        conf[self.rtype] = self.config
        with open(self.f,"w") as f:
            f.write(dict_for_file(conf))

    def __repr__(self):
        from resource_parse import dict_for_file
        return dict_for_file(self.config)

class Level(Resource):
    def __init__(self,dotfile):
        self.rtype = 'level'
        self.f = "presets/__template.level"
        self.load()

        self.f = dotfile
        self.load()

        assert(self.config)             
        self.gdkcolor = gtk.gdk.Color(256*self.backgroundcol[0],256*self.backgroundcol[1],256*self.backgroundcol[2])

    def save(self):
        self.instances = map(tuple,self.instances)
        self.backgroundcol = tuple(self.backgroundcol)
        Resource.save(self)

class Sprite(Resource):
    def __init__(self,dotfile):
        self.f = dotfile
        self.rtype = 'sprite'
        self.load()
        
        from PIL import Image
        self.imagedims = Image.open(self.image).size
        
    def framesize(self):
        w,h = self.framewidth, self.frameheight
        if w == -1:
            w = self.imagedims[0]
        if h == -1:
            h = self.imagedims[1]
        return (w,h)
        

def new_pixbuf(w,h, hasalpha=True):
    return gtk.gdk.Pixbuf(gdk.COLORSPACE_RGB,hasalpha,8,w,h)

class Object(Resource):
    def __init__(self,dotfile):
        self.f = dotfile
        self.rtype = 'object'
        self.load()
        self.w, self.h = 0,0
        if 'sprite' in self.config:
            self.spritefile = Sprite('/home/okke/projects/gamecreator/games/pikkenzuiger/sprites/' + self.sprite + '.sprite')
            parimage = gtk.gdk.pixbuf_new_from_file(self.spritefile.image)
            w,h = self.spritefile.framesize()
            self.w, self.h = w,h
            self.imagepreview = new_pixbuf(w,h)
            parimage.scale(self.imagepreview, 0, 0, w, h, 0, 0, 1, 1, gdk.INTERP_NEAREST)
        else:
            self.imagepreview = None
        
    def draw(self, gc, area, x, y):
        x = int(x)
        y = int(y)
        area.draw_pixbuf(gc, self.imagepreview, 
                         0, 0,
                         x,y)

gobject.type_register(LevelInfo)
gobject.type_register(ObjectInfo)
gobject.type_register(LevelPreview)
led = LevelEditor('games/pikkenzuiger/levels/eerste.level')
gtk.main()
