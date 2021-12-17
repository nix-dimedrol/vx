#ifndef VX_GL_VERTEX_HPP
#define VX_GL_VERTEX_HPP


#include "common.hpp"


namespace vx
{
namespace gl
{

__VX_GL_DECL_PROC(gen_buffers, GLvoid(*)(GLsizei, GLuint*), "glGenBuffers");
__VX_GL_DECL_PROC(delete_buffers, GLvoid(*)(GLsizei, GLuint const *), "glDeleteBuffers");
__VX_GL_DECL_PROC(bind_buffer, GLvoid(*)(GLenum, GLuint), "glBindBuffer");
__VX_GL_DECL_PROC(buffer_data, GLvoid(*)(GLenum, GLsizeiptr, GLvoid const *, GLenum), "glBufferData");
__VX_GL_DECL_PROC(buffer_sub_data, GLvoid(*)(GLenum, GLintptr, GLsizeiptr, GLvoid const *), "glBufferSubData");


using __buffer_types_pack = types_pack<__impl_gen_buffers, __impl_delete_buffers, __impl_bind_buffer,
	__impl_buffer_data, __impl_buffer_sub_data>;


__VX_GL_DECL_PROC(gen_vertex_arrays, GLvoid(*)(GLsizei, GLuint*), "glGenVertexArrays");
__VX_GL_DECL_PROC(bind_vertex_array, GLvoid(*)(GLuint), "glBindVertexArray");

__VX_GL_DECL_PROC(vertex_attrib_pointer, GLvoid(*)(GLuint, GLint, GLenum, GLboolean, GLsizei, GLvoid const *), "glVertexAttribPointer");
__VX_GL_DECL_PROC(vertex_attrib_i_pointer, GLvoid(*)(GLuint, GLint, GLenum, GLsizei, GLvoid const *), "glVertexAttribIPointer");

__VX_GL_DECL_PROC(enable_vertex_attrib_array, GLvoid(*)(GLuint), "glEnableVertexAttribArray");
__VX_GL_DECL_PROC(disable_vertex_attrib_array, GLvoid(*)(GLuint), "glDisableVertexAttribArray");


using __vertex_types_pack = types_pack<__impl_gen_vertex_arrays, __impl_bind_vertex_array,
	__impl_vertex_attrib_pointer, __impl_vertex_attrib_i_pointer,
	__impl_enable_vertex_attrib_array, __impl_disable_vertex_attrib_array>;


template<typename _Predicate>
void load_vertex_procs(_Predicate _pred)
{
	__load_procs(_pred, __buffer_types_pack{});
	__load_procs(_pred, __vertex_types_pack{});
}



template<typename _T>
struct VBO : __handling_entity<GLuint, VBO<_T>>
{
	explicit VBO(_T const * _data, size_t _n, GLenum _spec)
	{
		_tfunc<__impl_gen_buffers>::proc(1, &this->handle());
		this->use();
		_tfunc<__impl_buffer_data>::proc(GL_ARRAY_BUFFER, sizeof (_T) * _n, _data, _spec);
	}

	explicit VBO(size_t _n, GLenum _spec) : VBO(nullptr, _n, _spec) {}

	void load(_T const * _data, size_t _n, size_t _offset = 0)
	{
		_tfunc<__impl_buffer_sub_data>::proc(GL_ARRAY_BUFFER, _offset * sizeof (_T),
			_n * sizeof (_T), _data);
	}

	VBO<_T>& setup_attribute(GLuint _index, GLint _size, GLenum _spec, size_t _offset,
		bool _normalize = false)
	{
		_tfunc<__impl_vertex_attrib_pointer>::proc(_index, _size, _spec, _normalize, sizeof (_T),
			reinterpret_cast<void*>(_offset));
		return *this;
	}

	void use(void) { _tfunc<__impl_bind_buffer>::proc(GL_ARRAY_BUFFER, this->handle()); }

	static void destroy_resource(GLuint _val) { _tfunc<__impl_delete_buffers>::proc(1, &_val); }
};


template<size_t _N>
void enable_attributes(GLuint (&_arr)[_N])
{ for (auto it : _arr) _tfunc<__impl_enable_vertex_attrib_array>::proc(it); }

void enable_attributes(size_t _count)
{ for (size_t i{}; i < _count; i++) _tfunc<__impl_enable_vertex_attrib_array>::proc(i); }


template<size_t _N>
void disable_attributes(GLuint (&_arr)[_N])
{ for (auto it : _arr) _tfunc<__impl_disable_vertex_attrib_array>::proc(it); }

void disable_attributes(size_t _count)
{ for (size_t i{}; i < _count; i++) _tfunc<__impl_disable_vertex_attrib_array>::proc(i); }

} // namespace gl
} // namespace vx

#endif // VX_GL_VERTEX_HPP
