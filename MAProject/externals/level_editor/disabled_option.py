import bpy

from .add_disabled_option import MYADDON_OT_add_disabled_option

#パネル　ファイル名
class OBJECT_PT_disabled_option(bpy.types.Panel):
    """オブジェクトの切り替えフラグ"""
    bl_idname = "OBJECT_PT_disabled_option"
    bl_label = "DisabledOption"
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "object"

    #サブメニューの描画
    def draw(self, context):

        #パネルに項目を追加
      if "disabled_option" in context.object:
          #既にプロパティがあれば、プロパティを表示
          self.layout.prop(context.object, '["disabled_option"]',text=self.bl_label)
      else:
          #プロパティがなければ、プロパティ追加ボタンを表示
          self.layout.operator(MYADDON_OT_add_disabled_option.bl_idname)