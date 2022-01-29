#ifndef VX_GL_SHADER_HPP
#define VX_GL_SHADER_HPP


#include "common.hpp"
#include "error.hpp"
#include "uniform.hpp"
#include <vector>


namespace vx
{
namespace gl
{

__VX_GL_DECL_PROC(create_shader, GLuint(*)(GLenum), "glCreateShader");
__VX_GL_DECL_PROC(delete_shader, GLvoid(*)(GLuint), "glDeleteShader");
__VX_GL_DECL_PROC(shader_source, GLvoid(*)(GLuint, GLsizei, GLchar const **, GLint const *), "glShaderSource");
__VX_GL_DECL_PROC(compile_shader, GLvoid(*)(GLuint), "glCompileShader");
__VX_GL_DECL_PROC(get_shader_iv, GLvoid(*)(GLuint, GLenum, GLint*), "glGetShaderiv");
__VX_GL_DECL_PROC(get_shader_infolog, GLvoid(*)(GLuint, GLsizei, GLsizei*, GLchar*), "glGetShaderInfoLog");


using __shader_types_pack = types_pack<__impl_create_shader, __impl_delete_shader, __impl_shader_source,
	__impl_compile_shader, __impl_get_shader_iv, __impl_get_shader_infolog>;


__VX_GL_DECL_PROC(create_program, GLuint(*)(GLvoid), "glCreateProgram");
__VX_GL_DECL_PROC(delete_program, GLvoid(*)(GLuint), "glDeleteProgram");
__VX_GL_DECL_PROC(attach_shader, GLvoid(*)(GLuint, GLuint), "glAttachShader");
__VX_GL_DECL_PROC(detach_shader, GLvoid(*)(GLuint, GLuint), "glDetachShader");
__VX_GL_DECL_PROC(link_program, GLvoid(*)(GLuint), "glLinkProgram");
__VX_GL_DECL_PROC(use_program, GLvoid(*)(GLuint), "glUseProgram");
__VX_GL_DECL_PROC(get_program_iv, GLvoid(*)(GLuint, GLenum, GLint*), "glGetProgramiv");
__VX_GL_DECL_PROC(get_program_infolog, GLvoid(*)(GLuint, GLsizei, GLsizei*, GLchar*), "glGetProgramInfoLog");
__VX_GL_DECL_PROC(get_uniform_location, GLint(*)(GLuint, GLchar const *), "glGetUniformLocation");

__VX_GL_DECL_PROC(program_parameter_i, GLvoid(*)(GLuint, GLenum, GLint), "glProgramParameteri");


using __program_types_pack = types_pack<__impl_create_program, __impl_delete_program,
	__impl_attach_shader, __impl_detach_shader, __impl_link_program, __impl_use_program,
	__impl_get_program_iv, __impl_get_program_infolog, __impl_get_uniform_location,
	__impl_program_parameter_i>;


namespace detail
{

template<typename _InputIter>
void __set_shader_source(GLuint _handle, _InputIter _begin, _InputIter _end)
{
	size_t _src_cnt = std::distance(_begin, _end);
	std::vector<GLint> _length_arr;
	_length_arr.reserve(_src_cnt);
	std::vector<char const *> _data_arr;
	_data_arr.reserve(_src_cnt);
	for (auto it = _begin; it != _end; it++)
	{
		string_view const & _view = *it;
		_length_arr.push_back(_view.size());
		_data_arr.push_back(_view.data());
	}
	_tfunc<__impl_shader_source>::proc(_handle,
		_src_cnt, _data_arr.data(), _length_arr.data());
}

void __set_shader_source(GLuint _handle, string_view const & _src)
{
	GLint src_size = _src.size();
	GLchar const * src_data = _src.data();
	_tfunc<__impl_shader_source>::proc(_handle, 1, &src_data, &src_size);
}

void __compile_shader(GLuint _handle, std::error_code & _ec)
{
	_tfunc<__impl_compile_shader>::proc(_handle);
	GLint is_compiled{};
	_tfunc<__impl_get_shader_iv>::proc(_handle, GL_COMPILE_STATUS, &is_compiled);
	if (!is_compiled) _ec = error::shader_compilation_failure;
}

} // namespace detail


struct shader : noncopyable
{
	template<typename _InputIter>
	explicit shader(GLenum _spec, _InputIter _begin, _InputIter _end,
		std::error_code & _ec)
		: shader(_spec)
	{
		detail::__set_shader_source(handle, _begin, _end);
		detail::__compile_shader(handle, _ec);
	}

	template<typename _InputIter>
	explicit shader(GLenum _spec, _InputIter _begin, _InputIter _end)
		: shader(_spec)
	{
		std::error_code _ec;
		detail::__set_shader_source(handle, _begin, _end);
		detail::__compile_shader(handle, _ec);
		if (_ec) throw std::system_error{_ec};
	}

	explicit shader(GLenum _spec, std::initializer_list<string_view> _arr,
		std::error_code & _ec)
		: shader(_spec, _arr.begin(), _arr.end(), _ec) {}

	explicit shader(GLenum _spec, std::initializer_list<string_view> _arr)
		: shader(_spec, _arr.begin(), _arr.end()) {}


	explicit shader(GLenum _spec, string_view const & _src,
		std::error_code & _ec)
		: shader(_spec)
	{
		detail::__set_shader_source(handle, _src);
		detail::__compile_shader(handle, _ec);
	}

	explicit shader(GLenum _spec, string_view const & _src)
		: shader(_spec)
	{
		std::error_code _ec;
		detail::__set_shader_source(handle, _src);
		detail::__compile_shader(handle, _ec);
		if (_ec) throw std::system_error{_ec};
	}

	template<typename _Alloc = std::allocator<char>>
	auto log(_Alloc _alloc = _Alloc{})
	{
		GLint log_length{};
		_tfunc<__impl_get_shader_iv>::proc(handle, GL_INFO_LOG_LENGTH, &log_length);
		std::basic_string<char, std::char_traits<char>, _Alloc>
			_str(log_length, 0, _alloc);
		_tfunc<__impl_get_shader_infolog>::proc(handle,
			log_length, &log_length, const_cast<char*>(_str.c_str()));
		return std::move(_str);
	}

	~shader(void) { if (handle) _tfunc<__impl_delete_shader>::proc(handle); }

	constexpr operator GLuint(void) const noexcept { return handle; }

protected:

	explicit shader(GLenum _spec)
		: handle(_tfunc<__impl_create_shader>::proc(_spec)) {}

private:

	GLuint handle;
};


struct program : noncopyable
{
	explicit program(void)
		: handle(_tfunc<__impl_create_program>::proc()) {}

#ifdef VX_GL_PROGRAM_SEPARABLE_EXT
	template<typename _Iter>
	program & link(_Iter _begin, _Iter _end, bool _separable)
	{
		_tfunc<__impl_program_parameter_i>::proc(handle, GL_PROGRAM_SEPARABLE, _separable);
		return this->link(_begin, _end);
	}

	program & link(std::initializer_list<GLuint> _arr, bool _separable)
	{
		return this->link(_arr.begin(), _arr.end(), _separable);
	}
#endif


	template<typename _Iter>
	program & link(_Iter _begin, _Iter _end, std::error_code & _ec)
	{
		for (auto it = _begin; it != _end; it++)
			_tfunc<__impl_attach_shader>::proc(handle, *it);
		_tfunc<__impl_link_program>::proc(handle);
		GLint is_linked{};
		_tfunc<__impl_get_program_iv>::proc(handle, GL_LINK_STATUS, &is_linked);
		if (!is_linked) _ec = error::program_link_failure;
		for (auto it = _begin; it != _end; it++)
			_tfunc<__impl_detach_shader>::proc(handle, *it);
		return *this;
	}

	template<typename _Iter>
	program & link(_Iter _begin, _Iter _end)
	{
		std::error_code _ec;
		this->link(_begin, _end, _ec);
		if (_ec) throw std::system_error{_ec};
		return *this;
	}

	program & link(std::initializer_list<GLuint> _arr,
		std::error_code & _ec)
	{
		return this->link(_arr.begin(), _arr.end(), _ec);
	}

	program & link(std::initializer_list<GLuint> _arr)
	{
		return this->link(_arr.begin(), _arr.end());
	}

	template<typename _Alloc = std::allocator<char>>
	auto log(_Alloc _alloc = _Alloc{})
	{
		GLint log_length{};
		_tfunc<__impl_get_program_iv>::proc(handle, GL_INFO_LOG_LENGTH, &log_length);
		std::basic_string<char, std::char_traits<char>, _Alloc>
			_str(log_length, 0, _alloc);
		_tfunc<__impl_get_program_infolog>::proc(handle,
			log_length, &log_length, const_cast<char*>(_str.c_str()));
		return std::move(_str);
	}

	program & use(void)
	{
		_tfunc<__impl_use_program>::proc(handle);
		return *this;
	}

	GLint get_uniform_location(string_view const & _name)
	{ return _tfunc<__impl_get_uniform_location>::proc(handle, _name.data()); }

	~program(void) { if (handle) _tfunc<__impl_delete_program>::proc(handle); }

	constexpr operator GLuint(void) const noexcept { return handle; }

#ifdef VX_GL_DSA
	template<typename _T, length_t _N>
	enable_if_uniform_valid_t<vec<_T, _N>> uniform(GLint _loc,
		vec<_T, _N> const & _v)
	{
		_tfunc<detail::__impl_program_uniform<vec<_T, _N>>>::proc(handle,
			_loc, 1, reinterpret_cast<_T const *>(&_v));
	}

	template<typename _T, length_t _N>
	enable_if_uniform_valid_t<vec<_T, _N>> uniform(GLint _loc,
		vec<_T, _N> const * _arr, size_t _cnt)
	{
		_tfunc<detail::__impl_program_uniform<vec<_T, _N>>>::proc(handle,
			_loc, _cnt, reinterpret_cast<_T const *>(_arr));
	}

	template<typename _T, length_t _C, length_t _R>
	enable_if_uniform_valid_t<mat<_T, _C, _R>> uniform(GLint _loc,
		mat<_T, _C, _R> const & _m, bool _trsp = false)
	{
		_tfunc<detail::__impl_program_uniform<mat<_T, _C, _R>>>::proc(handle,
			_loc, 1, _trsp, reinterpret_cast<_T const *>(&_m));
	}

	template<typename _T, length_t _C, length_t _R>
	enable_if_uniform_valid_t<mat<_T, _C, _R>> uniform(GLint _loc,
		mat<_T, _C, _R> const * _arr, size_t _cnt, bool _trsp = false)
	{
		_tfunc<detail::__impl_program_uniform<mat<_T, _C, _R>>>::proc(handle,
			_loc, _cnt, _trsp, reinterpret_cast<_T const *>(_arr));
	}

	template<typename _T>
	enable_if_uniform_valid_t<_T> uniform(GLint _loc,
		_T _val)
	{
		_tfunc<detail::__impl_program_uniform<vec<_T, 1>>>::proc(handle,
			_loc, 1, &_val);
	}

	template<typename _T>
	enable_if_uniform_valid_t<_T> uniform(GLint _loc,
		_T const * _arr, size_t _cnt)
	{
		_tfunc<detail::__impl_program_uniform<vec<_T, 1>>>::proc(handle,
			_loc, _cnt, _arr);
	}
#endif

private:

	GLuint handle;
};

} // namespace gl
} // namespace vx

#endif // VX_GL_SHADER_HPP
