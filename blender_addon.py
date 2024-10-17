bl_info = {
    "name": "Custom Exporter",
    "blender": (2, 80, 0),
    "category": "Import-Export",
}

import bpy
import struct
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty
from bpy.types import Operator

def export(filepath, obj):
    print(f"Exporting {filepath}")

    # Ensure object mode so the uv's always get exported
    if bpy.context.object.mode != 'OBJECT':
        bpy.ops.object.mode_set(mode='OBJECT')

    # obj = bpy.context.object
    if obj.type != "MESH":
        print("Selected object is not a mesh")
        return {"CANCELLED"}
    
    mesh = obj.data
    mesh.calc_loop_triangles()
    
    # uv_layer = mesh.uv_layers.active.data if mesh.uv_layers.active else None
    uv_layer = mesh.uv_layers.active
    if not uv_layer:
        print("No active UV layer")
        return {"CANCELLED"}
    else:
        uv_layer = uv_layer.data
    
    vertex_uv_map = {}
    vertex_list = []
    vertex_index_map = {}
    current_index = 0
    
    # Collect unique vertices with UVs
    for loop in mesh.loops:
        vert = mesh.vertices[loop.vertex_index]
        pos = (vert.co.x, vert.co.y, vert.co.z)
        
        if uv_layer:
            uv = (uv_layer[loop.index].uv.x, uv_layer[loop.index].uv.y)
        else:
            uv = (0.0, 0.0)
        
        vertex_key = (pos, uv)
        
        # If the vertex/uv pair is not already in the map, add it
        if vertex_key not in vertex_uv_map:
            vertex_uv_map[vertex_key] = current_index
            vertex_list.append((pos, uv))
            current_index += 1
        
        # Map loop index to the vertex/uv pair index
        vertex_index_map[loop.index] = vertex_uv_map[vertex_key]
    
    with open(filepath, "wb") as file:
        # I: u32
        vertex_count = len(vertex_list)
        print(f"Vertex count: {vertex_count}")
        file.write(struct.pack("I", vertex_count))  # 4 bytes
        
        for vert, uv in vertex_list:
            pos = vert
            file.write(struct.pack("fff ff", pos[0], pos[2], pos[1], uv[0], uv[1]))  # 3 floats (pos) + 2 floats (UV)
        
        # I: u32
        index_count = len(mesh.loop_triangles) * 3
        print(f"Triangle count: {index_count}")
        file.write(struct.pack("I", index_count))
        
        # Write each triangle"s reindexed vertex indices
        for triangle in mesh.loop_triangles:
            file.write(struct.pack("III", vertex_index_map[triangle.loops[0]], vertex_index_map[triangle.loops[1]], vertex_index_map[triangle.loops[2]]))
    
    print(f"Exported custom mesh data to {filepath}")
    return {"FINISHED"}

# Operator class to add to the Export menu
class ExportHerreMesh(Operator, ExportHelper):
    bl_idname = "export_mesh.mesh"
    bl_label = "Export Custom Mesh"
    
    # Filter file extensions
    filename_ext = ".mesh"
    
    # Property for file path
    filepath: StringProperty(subtype="FILE_PATH")
    
    def execute(self, context):
        # Call the mesh export function
        context.view_layer.update()
        return export(self.filepath, context.active_object)

# Register the operator and add it to the Export menu
def menu_func_export(self, context):
    self.layout.operator(ExportHerreMesh.bl_idname, text="Custom Mesh (.mesh)")

# Register and unregister functions
def register():
    bpy.utils.register_class(ExportHerreMesh)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)

def unregister():
    bpy.utils.unregister_class(ExportHerreMesh)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)

if __name__ == "__main__":
    register()