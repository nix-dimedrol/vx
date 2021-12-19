#ifndef VX_GL_VERTEX_HPP
#define VX_GL_VERTEX_HPP


#include "common.hpp"
#include "../math_types.hpp"


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


namespace detail
{

template<typename>
struct __tmacro_type
{
	static const GLenum value;
};

template<> const GLenum __tmacro_type<float>::value         = GL_FLOAT;
template<> const GLenum __tmacro_type<double>::value        = GL_DOUBLE;
template<> const GLenum __tmacro_type<std::int32_t>::value  = GL_INT;
template<> const GLenum __tmacro_type<std::uint32_t>::value = GL_UNSIGNED_INT;
template<> const GLenum __tmacro_type<std::int16_t>::value  = GL_SHORT;
template<> const GLenum __tmacro_type<std::uint16_t>::value = GL_UNSIGNED_SHORT;
template<> const GLenum __tmacro_type<std::int8_t>::value   = GL_BYTE;
template<> const GLenum __tmacro_type<std::uint8_t>::value  = GL_UNSIGNED_BYTE;


template<size_t _VDataSize, typename _T, size_t _N>
struct __config_attrib_impl
{
	static void setup(size_t _idx, size_t _offset)
	{
		_tfunc<__impl_vertex_attrib_pointer>::proc(_idx, _N, __tmacro_type<_T>::value, false,
			_VDataSize, reinterpret_cast<void*>(_offset));
	}
};


template<size_t _VDataSize, typename _T>
struct __config_attrib : __config_attrib_impl<_VDataSize, _T, 1> {};

template<size_t _VDataSize, typename _T, length_t _N>
struct __config_attrib<_VDataSize, vec<_T, _N>> : __config_attrib_impl<_VDataSize, _T, _N> {};


template<size_t, typename...> struct __config_attrib_recurse
{ static void setup(size_t, size_t) {} };
template<size_t _VDataSize, typename _T, typename... _Args>
struct __config_attrib_recurse<_VDataSize, _T, _Args...>
{
	static void setup(size_t _idx = 0, size_t _offset = 0)
	{
		__config_attrib<_VDataSize, _T>::setup(_idx, _offset);
		__config_attrib_recurse<_VDataSize, _Args...>::setup(_idx + 1, _offset + sizeof (_T));
	}
};

} // namespace detail



template<typename _T>
struct VBO : noncopyable
{
	using value_type = _T;
	using self_type  = VBO<value_type>;

	explicit VBO(value_type const * _data, size_t _n, GLenum _spec)
	{
		_tfunc<__impl_gen_buffers>::proc(1, &handle);
		this->use();
		_tfunc<__impl_buffer_data>::proc(GL_ARRAY_BUFFER,
			sizeof (value_type) * _n, _data, _spec);
	}

	explicit VBO(size_t _n, GLenum _spec) : VBO(nullptr, _n, _spec) {}


	self_type & load(_T const * _data, size_t _n, size_t _offset = 0)
	{
		_tfunc<__impl_buffer_sub_data>::proc(GL_ARRAY_BUFFER,
			_offset * sizeof (value_type), _n * sizeof (_T), _data);
		return *this;
	}

	self_type & setup_attrib(GLuint _index, GLint _size, GLenum _spec, size_t _offset,
		bool _normalize = false)
	{
		_tfunc<__impl_vertex_attrib_pointer>::proc(_index, _size, _spec, _normalize,
			sizeof (value_type), reinterpret_cast<void*>(_offset));
		return *this;
	}

	template<typename ..._Args>
	self_type & setup_attribs(void)
	{
		detail::__config_attrib_recurse<sizeof(_T), _Args...>::setup();
		return *this;
	}

	self_type & use(void)
	{
		_tfunc<__impl_bind_buffer>::proc(GL_ARRAY_BUFFER, handle);
		return *this;
	}

	~VBO(void) { _tfunc<__impl_delete_buffers>::proc(1, &handle); }

private:

	GLuint handle;
};


template<size_t _N>
void enable_attribs(GLuint (&_arr)[_N])
{ for (auto it : _arr) _tfunc<__impl_enable_vertex_attrib_array>::proc(it); }

void enable_attribs(size_t _count)
{ for (size_t i{}; i < _count; i++) _tfunc<__impl_enable_vertex_attrib_array>::proc(i); }


template<size_t _N>
void disable_attribs(GLuint (&_arr)[_N])
{ for (auto it : _arr) _tfunc<__impl_disable_vertex_attrib_array>::proc(it); }

void disable_attribs(size_t _count)
{ for (size_t i{}; i < _count; i++) _tfunc<__impl_disable_vertex_attrib_array>::proc(i); }

} // namespace gl
} // namespace vx

#endif // VX_GL_VERTEX_HPP
