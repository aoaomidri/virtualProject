import bpy 

#オペレータ　カスタムプロパティ['animation_name']追加
class MYADDON_OT_AddAnimationName(bpy.types.Operator):
    bl_idname = "myaddon.add_animation_name"
    bl_label = "Add Animation Name"
    bl_description = "Add 'animation_name' custom property"
    bl_options = {"REGISTER", "UNDO"}

    def execute(self, context):
        # Add 'animation_name' custom property
        context.object["animation_name"] = ""
        return {"FINISHED"}