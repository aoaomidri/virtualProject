import bpy
from .AddAnimationName import MYADDON_OT_AddAnimationName

class OBJECT_PT_AnimationName(bpy.types.Panel):
    """Object Animation Name Panel"""
    bl_idname = "OBJECT_PT_animation_name"
    bl_label = "Animation Name"
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "object"

    def draw(self, context):
        layout = self.layout

        if "animation_name" in context.object:
            # Display the property if it already exists
            layout.prop(context.object, '["animation_name"]', text=self.bl_label)
        else:
            # Display a button to add the property if it doesn't exist
            layout.operator(MYADDON_OT_AddAnimationName.bl_idname) 