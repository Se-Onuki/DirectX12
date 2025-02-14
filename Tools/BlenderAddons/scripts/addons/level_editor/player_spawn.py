import bpy
import os # モデルファイルのパスを取得するために使用

# オペレータ 出現ポイントのシンボルを読み込む
class MYADDON_OT_player_spawn_model(bpy.types.Operator):
	bl_idname = "myaddon.myaddon_ot_player_spawn_model"
	bl_label = "プレイヤー出現ポイントImport"
	bl_description = "プレイヤー出現ポイントを読み込みます"

	prototype_object_name = "prottype_player_spawn_point"
	object_name = "player_spawn_point"

	def execute(self, context):
		print("プレイヤー出現ポイントを読み込みます")

		# 重複ロードの対策
		# 既にオブジェクトが存在しているのなら､取得する
		spawn_object = bpy.data.objects.get(MYADDON_OT_player_spawn_model.prototype_object_name)
		if spawn_object is not None:
			return {'CANCELLED'}


		# スクリプトが配置されているディレクトリの名前を取得
		addon_dir = os.path.dirname(__file__)
		# ディレクトリからのモデルファイルの相対パスを記述
		relative_path = "player_model/Orb.obj"
		# モデルファイルのフルパスを取得
		full_path = os.path.join(addon_dir, relative_path)
		# gltfメッシュを読み込む
		bpy.ops.wm.obj_import('EXEC_DEFAULT', filepath=full_path, display_type = 'THUMBNAIL', forward_axis = 'Z', up_axis = 'Y')
		# 回転の状態を適用
		bpy.ops.object.transform_apply(location=False, rotation=True, scale=False)

		# アクティブなオブジェクトを取得する
		object = bpy.context.active_object
		# オブジェクトの名前を変更
		object.name = MYADDON_OT_player_spawn_model.prototype_object_name

		# オブジェクトの種類を指定
		object["type"] = MYADDON_OT_player_spawn_model.object_name
		
		# メモリには残すが､シーンには表示しない
		bpy.context.collection.objects.unlink(object)

		return {"FINISHED"}
	
# オペレータ 出現ポイントの生成
class MYADDON_OT_player_spawn_point(bpy.types.Operator):
	bl_idname = "myaddon.myaddon_ot_player_spawn_point"
	bl_label = "プレイヤー出現ポイント生成"
	bl_description = "プレイヤー出現ポイントを生成します"
	# Redo Undoを可能にするオプション
	bl_options ={"REGISTER", "UNDO"}

	def execute(self, context):

		# 読み込み済みのコピー元オブジェクトを検索
		spawn_object = bpy.data.objects.get(MYADDON_OT_player_spawn_model.prototype_object_name)

		# まだ読み込んでいない場合
		if spawn_object is None:
			# 読み込みオペレータを実行する
			bpy.ops.myaddon.myaddon_ot_player_spawn_model('EXEC_DEFAULT')
			# 読み込み済みのコピー元オブジェクトを検索
			spawn_object = bpy.data.objects.get(MYADDON_OT_player_spawn_model.prototype_object_name)

		print("プレイヤー出現ポイントを生成します")

		# Blenderでの選択を解除する
		bpy.ops.object.select_all(action='DESELECT')

		# 複製元の非表示オブジェクトを複製する
		copy_object = spawn_object.copy()
		# 複製したオブジェクトを現在のシーンにリンク(出現させる)
		bpy.context.collection.objects.link(copy_object)
		# オブジェクト名を変更
		copy_object.name = MYADDON_OT_player_spawn_model.object_name

		return {"FINISHED"}