//#pragma once
//#include <unordered_map>
//#include <string>
//
//class MeshFace {
//constructor(mesh, data) {
//super(data);
//this.mesh = mesh;
//this.uv = {};
//this.texture = false;
//if (data) {
//this.extend(data);
//}
//}
//extend(data) {
//super.extend(data);
//this.vertices.forEach(key = > {
//if (!this.uv[key]) this.uv[key] = [0, 0];
//if (data.uv && data.uv[key] instanceof Array) {
//this.uv[key].replace(data.uv[key]);
//}
//})
//for (let key in this.uv) {
//if (!this.vertices.includes(key)) {
//delete this.uv[key];
//}
//}
//return this;
//}
//getNormal(normalize) {
//let vertices = this.getSortedVertices();
//if (vertices.length < 3) return[0, 0, 0];
//let a = [
//this.mesh.vertices[vertices[1]][0] - this.mesh.vertices[vertices[0]][0],
//this.mesh.vertices[vertices[1]][1] - this.mesh.vertices[vertices[0]][1],
//this.mesh.vertices[vertices[1]][2] - this.mesh.vertices[vertices[0]][2],
//]
//let b = [
//this.mesh.vertices[vertices[2]][0] - this.mesh.vertices[vertices[0]][0],
//this.mesh.vertices[vertices[2]][1] - this.mesh.vertices[vertices[0]][1],
//this.mesh.vertices[vertices[2]][2] - this.mesh.vertices[vertices[0]][2],
//]
//let direction = [
//a[1] * b[2] - a[2] * b[1],
//a[2] * b[0] - a[0] * b[2],
//a[0] * b[1] - a[1] * b[0],
//]
//if (normalize) {
//let length = Math.sqrt(direction[0] * direction[0] + direction[1] * direction[1] + direction[2] * direction[2]);
//return direction.map(dir = > dir / length || 0);
//}
//else {
//return direction
//}
//}
//getBoundingRect() {
//let min_x = Project.texture_width, min_y = Project.texture_height, max_x = 0, max_y = 0;
//this.vertices.forEach(vkey = > {
//min_x = Math.min(min_x, this.uv[vkey][0]); max_x = Math.max(max_x, this.uv[vkey][0]);
//min_y = Math.min(min_y, this.uv[vkey][1]); max_y = Math.max(max_y, this.uv[vkey][1]);
//})
//return getRectangle(min_x, min_y, max_x, max_y);
//}
//getOccupationMatrix(texture_space = false, start_offset = [0, 0], matrix = {}) {
//let face = this;
//let rect = this.getBoundingRect();
//let texture = texture_space && this.getTexture();
//let sorted_vertices = this.getSortedVertices();
//let factor_x = texture ? (texture.width / Project.texture_width) : 1;
//let factor_y = texture ? (texture.display_height / Project.texture_height) : 1;
//
//if (texture_space && texture) {
//rect.ax *= factor_x;
//rect.ay *= factor_y;
//rect.bx *= factor_x;
//rect.by *= factor_y;
//}
//function vSub(a, b) {
//return[a[0] - b[0], a[1] - b[1]];
//}
//function getSide(a, b) {
//let cosine_sign = a[0] * b[1] - a[1] * b[0];
//if (cosine_sign > 0) return 1;
//if (cosine_sign < 0) return -1;
//}
//function pointInsidePolygon(x, y) {
//let previous_side;
//let i = 0;
//for (let vkey of sorted_vertices) {
//let a = face.uv[vkey];
//let b = face.uv[sorted_vertices[i + 1]] || face.uv[sorted_vertices[0]];
//if (factor_x != = 1 || factor_y != = 1) {
//a = a ? [a[0] * factor_x, a[1] * factor_y] : [0, 0] ;
//b = b ? [b[0] * factor_x, b[1] * factor_y] : [0, 0] ;
//}
//
//let affine_segment = vSub(b, a);
//let affine_point = vSub([x, y], a);
//let side = getSide(affine_segment, affine_point);
//if (!side) return false;
//if (!previous_side) previous_side = side;
//if (side != = previous_side) return false;
//i++;
//}
//return true;
//}
//for (let x = Math.floor(rect.ax); x < Math.ceil(rect.bx); x++) {
//for (let y = Math.floor(rect.ay); y < Math.ceil(rect.by); y++) {
//let matrix_x = x - start_offset[0];
//let matrix_y = y - start_offset[1];
//
//let inside = (pointInsidePolygon(x + 0.00001, y + 0.00001)
//|| pointInsidePolygon(x + 0.99999, y + 0.00001)
//|| pointInsidePolygon(x + 0.00001, y + 0.99999)
//|| pointInsidePolygon(x + 0.99999, y + 0.99999));
//if (!inside) {
//let i = 0;
//let px_rect = [[x, y], [x + 0.99999, y + 0.99999]]
//for (let vkey of sorted_vertices) {
//let vkey_b = sorted_vertices[i + 1] || sorted_vertices[0]
//if (pointInRectangle(face.uv[vkey], ...px_rect)) {
//inside = true; break;
//}
//if (lineIntersectsReactangle(face.uv[vkey], face.uv[vkey_b], ...px_rect)) {
//inside = true; break;
//}
//i++;
//}
//}
//if (inside) {
//if (!matrix[matrix_x]) matrix[matrix_x] = {};
//matrix[matrix_x][matrix_y] = true;
//}
//}
//}
//return matrix;
//}
//getUVIsland() {
//let keys = [this.getFaceKey()];
//function crawl(face) {
//for (let i = 0; i < face.vertices.length; i++) {
//let adjacent = face.getAdjacentFace(i);
//if (!adjacent) continue;
//if (keys.includes(adjacent.key)) continue;
//let epsilon = 0.2;
//let uv_a1 = adjacent.face.uv[adjacent.edge[0]];
//let uv_a2 = face.uv[adjacent.edge[0]];
//if (!Math.epsilon(uv_a1[0], uv_a2[0], epsilon) || !Math.epsilon(uv_a1[1], uv_a2[1], epsilon)) continue;
//let uv_b1 = adjacent.face.uv[adjacent.edge[1]];
//let uv_b2 = face.uv[adjacent.edge[1]];
//if (!Math.epsilon(uv_b1[0], uv_b2[0], epsilon) || !Math.epsilon(uv_b1[1], uv_b2[1], epsilon)) continue;
//keys.push(adjacent.key);
//crawl(adjacent.face);
//}
//}
//crawl(this);
//return keys;
//}
//getAngleTo(other) {
//let a = new THREE.Vector3().fromArray(this.getNormal());
//let b = new THREE.Vector3().fromArray(other instanceof Array ? other : other.getNormal());
//return Math.radToDeg(a.angleTo(b));
//}
//invert() {
//if (this.vertices.length < 3) return this;
//[this.vertices[0], this.vertices[1]] = [this.vertices[1], this.vertices[0]];
//}
//isSelected(fkey) {
//return !!Project.mesh_selection[this.mesh.uuid] && Project.mesh_selection[this.mesh.uuid].faces.includes(fkey || this.getFaceKey());
//}
//getSortedVertices() {
//if (this.vertices.length < 4) return this.vertices;
//
//// Test if point "check" is on the other side of the line between "base1" and "base2", compared to "top"
//function test(base1, base2, top, check) {
//base1 = Canvas.temp_vectors[0].fromArray(base1);
//base2 = Canvas.temp_vectors[1].fromArray(base2);
//top = Canvas.temp_vectors[2].fromArray(top);
//check = Canvas.temp_vectors[3].fromArray(check);
//
//// Construct a plane with coplanar points "base1" and "base2" with a normal towards "top"
//let normal = Canvas.temp_vectors[4];
//new THREE.Line3(base1, base2).closestPointToPoint(top, false, normal);
//normal.sub(top);
//let plane = new THREE.Plane().setFromNormalAndCoplanarPoint(normal, base2);
//let distance = plane.distanceToPoint(check);
//return distance > 0;
//}
//let{ mesh, vertices } = this;
//
//if (test(mesh.vertices[vertices[1]], mesh.vertices[vertices[2]], mesh.vertices[vertices[0]], mesh.vertices[vertices[3]])) {
//return[vertices[2], vertices[0], vertices[1], vertices[3]];
//
//}
//else if (test(mesh.vertices[vertices[0]], mesh.vertices[vertices[1]], mesh.vertices[vertices[2]], mesh.vertices[vertices[3]])) {
//return[vertices[0], vertices[2], vertices[1], vertices[3]];
//}
//return vertices;
//}
//getEdges() {
//let vertices = this.getSortedVertices();
//if (vertices.length == 2) {
//return vertices;
//}
//else if (vertices.length > 2) {
//return vertices.map((vkey1, i) = > {
//let vkey2 = vertices[i + 1] || vertices[0];
//return[vkey1, vkey2];
//})
//}
//else {
//return[];
//}
//}
//getAdjacentFace(side_index = 0) {
//let vertices = this.getSortedVertices();
//side_index = side_index % this.vertices.length;
//let side_vertices = [
//vertices[side_index],
//vertices[side_index + 1] || vertices[0]
//]
//for (let fkey in this.mesh.faces) {
//let face = this.mesh.faces[fkey];
//if (face == = this) continue;
//if (face.vertices.includes(side_vertices[0]) && face.vertices.includes(side_vertices[1])) {
//let f_vertices = face.getSortedVertices();
//let index_a = f_vertices.indexOf(side_vertices[0]);
//let index_b = f_vertices.indexOf(side_vertices[1]);
//if (index_b - index_a == -1 || (index_b - index_a == f_vertices.length - 1)) {
//return {
//face,
//key: fkey,
//index : index_b,
//edge : side_vertices
//}
//}
//}
//}
//return null;
//}
//getFaceKey() {
//for (let fkey in this.mesh.faces) {
//if (this.mesh.faces[fkey] == this) return fkey;
//}
//}
//UVToLocal(uv, vertices = this.getSortedVertices()) {
//let vert_a = vertices[0];
//let vert_b = vertices[1];
//let vert_c = vertices[2];
//
//if (vertices[3]) {
//let is_in_tri = pointInTriangle(uv, this.uv[vert_a], this.uv[vert_b], this.uv[vert_c]);
//
//if (!is_in_tri) {
//vert_a = vertices[0];
//vert_b = vertices[2];
//vert_c = vertices[3];
//}
//}
//let p0 = this.uv[vert_a];
//let p1 = this.uv[vert_b];
//let p2 = this.uv[vert_c];
//
//let vertexa = this.mesh.vertices[vert_a];
//let vertexb = this.mesh.vertices[vert_b];
//let vertexc = this.mesh.vertices[vert_c];
//
//let b0 = (p1[0] - p0[0]) * (p2[1] - p0[1]) - (p2[0] - p0[0]) * (p1[1] - p0[1])
//let b1 = ((p1[0] - uv[0]) * (p2[1] - uv[1]) - (p2[0] - uv[0]) * (p1[1] - uv[1])) / b0
//let b2 = ((p2[0] - uv[0]) * (p0[1] - uv[1]) - (p0[0] - uv[0]) * (p2[1] - uv[1])) / b0
//let b3 = ((p0[0] - uv[0]) * (p1[1] - uv[1]) - (p1[0] - uv[0]) * (p0[1] - uv[1])) / b0
//
//let local_space = new THREE.Vector3(
//vertexa[0] * b1 + vertexb[0] * b2 + vertexc[0] * b3,
//vertexa[1] * b1 + vertexb[1] * b2 + vertexc[1] * b3,
//vertexa[2] * b1 + vertexb[2] * b2 + vertexc[2] * b3,
//)
//return local_space;
//}
//localToUV(vector, vertices = this.vertices) {
//let va = new THREE.Vector3().fromArray(this.mesh.vertices[vertices[0]]);
//let vb = new THREE.Vector3().fromArray(this.mesh.vertices[vertices[1]]);
//let vc = new THREE.Vector3().fromArray(this.mesh.vertices[vertices[2]]);
//
//let uva = new THREE.Vector2().fromArray(this.uv[vertices[0]]);
//let uvb = new THREE.Vector2().fromArray(this.uv[vertices[1]]);
//let uvc = new THREE.Vector2().fromArray(this.uv[vertices[2]]);
//
//let uv = THREE.Triangle.getUV(vector, va, vb, vc, uva, uvb, uvc, new THREE.Vector2());
//return uv.toArray();
//}
//getCenter() {
//let center = [0, 0, 0];
//this.vertices.forEach(vkey = > {
//let vertex = this.mesh.vertices[vkey];
//center.V3_add(vertex);
//})
//center.V3_divide(this.vertices.length);
//return center;
//}
//}
