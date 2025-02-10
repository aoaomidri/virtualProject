import bpy 

#他クラスをimportする
from .stretch_vertex import MYADDON_OT_stretch_vertex
from .create_ico_sphere import MYADDON_OT_create_ico_sphere
from .export_scene import MYADDON_OT_export_scene

#ブレンダーに登録するアドオン情報
bl_info = {
    "name": "レベルエディタ",
    "author": "aoi murakami",
    "version": (1, 0),
    "blender": (3, 3, 1),
    "location": "",
    "description": "レベルエディタ",
    "warning": "",
    "wiki_url": "",
    "tracker_url": "",
    "category": "Object"
}

#トップバーの拡張メニュー
class TOPBAR_MT_my_menu(bpy.types.Menu):
    #Blenderがクラスを識別するための固有の文字列
    bl_idname = "TOPBAR_MT_my_menu"
    #メニューのラベルとして表示される文字列
    bl_label = "MyMenu"
    #著者表示用の文字列
    bl_description = "拡張メニュー by " + bl_info["author"]

    #サブメニューの描画
    def draw(self,context):
        #TOPバーの「エディターメニュー」に項目(オペレーター)を追加
        self.layout.operator(MYADDON_OT_stretch_vertex.bl_idname,
                             text = MYADDON_OT_stretch_vertex.bl_label)
        self.layout.operator(MYADDON_OT_create_ico_sphere.bl_idname,
                             text = MYADDON_OT_create_ico_sphere.bl_label)
        self.layout.operator(MYADDON_OT_export_scene.bl_idname,
                             text = MYADDON_OT_export_scene.bl_label)
    
    #既存のメニューにサブメニューを追加
    def submenu(self,context):

        #ID指定でサブメニューを追加
        self.layout.menu(TOPBAR_MT_my_menu.bl_idname)