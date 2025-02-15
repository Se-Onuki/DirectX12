import bpy
import bpy_extras
import os # モデルファイルのパスを取得するために使用
from .spawn import *

# オペレータ 出現ポイントの生成
class MYADDON_OT_player_spawn_point(bpy.types.Operator):
	bl_idname = "myaddon.myaddon_ot_player_spawn_point"
	bl_label = "プレイヤー出現ポイント生成"
	bl_description = "プレイヤー出現ポイントを生成します"
	# Redo Undoを可能にするオプション
	bl_options ={"REGISTER", "UNDO"}

	def execute(self, context):

		bpy.ops.myaddon.myaddon_ot_spawn_point_generate('EXEC_DEFAULT', type = "Player")

		return {"FINISHED"}
	
# オペレータ 出現ポイントの生成
class MYADDON_OT_enemy_spawn_point(bpy.types.Operator):
	bl_idname = "myaddon.myaddon_ot_enemy_spawn_point"
	bl_label = "敵出現ポイント生成"
	bl_description = "敵出現ポイントを生成します"
	# Redo Undoを可能にするオプション
	bl_options ={"REGISTER", "UNDO"}

	def execute(self, context):

		bpy.ops.myaddon.myaddon_ot_spawn_point_generate('EXEC_DEFAULT', type="Enemy")

		return {"FINISHED"}