import bpy

def delete_hierarchy(obj: bpy.types.Object):
	for child in obj.children:
		delete_hierarchy(child)
	bpy.data.objects.remove(obj)
