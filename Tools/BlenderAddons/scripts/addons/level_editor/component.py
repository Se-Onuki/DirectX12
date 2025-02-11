import bpy

# オペレータ カスタムプロパティ[ 'visiblity' ]追加
class MYADDON_OT_add_visibility(bpy.types.Operator):
	bl_idname = "myaddon.myaddon_ot_add_visibility"
	bl_label = "可視性フラグの追加"
	bl_description = "[ 'visiblity' ]カスタムプロパティを追加します"
	bl_options = {"REGISTER", "UNDO"}

	def execute(self, context):
		#[ 'visiblity' ]カスタムプロパティを追加
		context.object["visiblity"] = True
		return {"FINISHED"}


# オペレータ カスタムプロパティ['file_name']を追加
class MYADDON_OT_add_modeldata(bpy.types.Operator):
	bl_idname = "myaddon.myaddon_ot_add_modeldata"
	bl_label = "ModelData 追加"
	bl_description = "カスタムプロパティ['file_name']を追加します"
	bl_options = {"REGISTER", "UNDO"}

	def execute(self, context):
		
		# カスタムプロパティ['directory_name']を追加
		context.object["directory_name"] = ""
		# カスタムプロパティ['file_name']を追加
		context.object["file_name"] = ""

		return {"FINISHED"}

# オペレータ カスタムプロパティ['rigidbody']を追加
class MYADDON_OT_add_rigidbody(bpy.types.Operator):
	bl_idname = "myaddon.myaddon_ot_add_rigidbody"
	bl_label = "rigidbody 追加"
	bl_description = "カスタムプロパティ['rigidbody']を追加します"
	bl_options = {"REGISTER", "UNDO"}

	def execute(self, context):
		
		# カスタムプロパティ['rigidbody']を追加
		context.object["rigidbody"] = True
		context.object["isKinematic"] = True
		context.object["gravity"] = -9.8

		return {"FINISHED"}

