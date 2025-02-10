import bpy

#オペレータ　カスタムプロパティ['disabled_option']追加
class MYADDON_OT_add_disabled_option(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_add_disabled_option"
    bl_label = "disabled_option 追加"
    bl_description = "['disabled_option']カスタマプロパティを追加します"
    bl_options = {"REGISTER","UNDO"}

    def execute(self, context):

        #['disabled_option']カスタムプロパティを追加
        context.object["disabled_option"] = True

        return {"FINISHED"}  