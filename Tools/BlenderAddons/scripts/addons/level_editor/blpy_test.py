import bpy
from bpy.types import Context
import bpy_extras

# オペレータ 頂点を伸ばす
class MYADDON_OT_stretch_vertex(bpy.types.Operator):
	bl_idname = "myaddon.myaddon_ot_stretch_vertex"
	bl_label = "頂点を伸ばす"
	bl_description = "頂点座標を引っ張って伸ばします"
	# Redo Undoを可能にするオプション
	bl_options ={"REGISTER", "UNDO"}

	# メニューを実行するときに呼ばれるコールバック関数
	def execute(self, context: Context):
		bpy.data.objects["Cube"].data.vertices[0].co.y += 1.0
		print ("頂点を伸ばしました")

		# オペレータの命令終了を通知
		return {"FINISHED"}


# オペレータ ICO球の生成
class MYADDON_OT_create_ico_sphere(bpy.types.Operator):
	bl_idname = "myaddon.myaddon_ot_create_ico_sphere"
	bl_label = "ICO球の生成"
	bl_description = "ICO球を生成します"
	bl_options = {"REGISTER", "UNDO"}

	# メニューを実行するときに呼ばれるコールバック関数
	def execute(self, context):
		# ico球を生成 参考url: https://docs.blender.org/api/current/bpy.ops.mesh.html#bpy.ops.mesh.primitive_ico_sphere_add
		bpy.ops.mesh.primitive_ico_sphere_add()
		print ("ICO球を生成しました")

		# オペレータの命令終了を通知
		return {"FINISHED"}
	