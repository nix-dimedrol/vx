#ifndef VX_GL_SHADER_HPP
#define VX_GL_SHADER_HPP


#include "common.hpp"
#include "error.hpp"
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


template<typename _Predicate>
void load_shader_procs(_Predicate _pred)
{
	__load_procs(_pred, __shader_types_pack{});
	__load_procs(_pred, __program_types_pack{});
}


struct shader : noncopyable
{
	template<typename _InputIter>
	explicit shader(GLenum _spec, _InputIter _begin, _InputIter _end, std::error_code & _ec)
		: shader(_spec)
	{
		this->__compile_source(_begin, _end, _ec);
	}

	template<typename _InputIter>
	explicit shader(GLenum _spec, _InputIter _begin, _InputIter _end)
		: shader(_spec)
	{
		std::error_code _ec;
		this->__compile_source(_begin, _end, _ec);
		if (_ec) throw std::system_error{_ec};
	}

	explicit shader(GLenum _spec, std::initializer_list<string_view> _arr, std::error_code & _ec)
		: shader(_spec, _arr.begin(), _arr.end(), _ec) {}

	explicit shader(GLenum _spec, std::initializer_list<string_view> _arr)
		: shader(_spec, _arr.begin(), _arr.end()) {}


	explicit shader(GLenum _spec, string_view const & _src, std::error_code & _ec)
		: shader(_spec)
	{
		this->__compile_source(_src, _ec);
	}

	explicit shader(GLenum _spec, string_view const & _src)
		: shader(_spec)
	{
		std::error_code _ec;
		this->__compile_source(_src, _ec);
		if (_ec) throw std::system_error{_ec};
	}

	template<typename _Logbuf>
	shader & get_log(_Logbuf & _log)
	{
		GLint log_length{};
		_tfunc<__impl_get_shader_iv>::proc(handle, GL_INFO_LOG_LENGTH, &log_length);
		_tfunc<__impl_get_shader_infolog>::proc(handle,
			log_length, &log_length, _log.get(log_length));
	}

	~shader(void) { if (handle) _tfunc<__impl_delete_shader>::proc(handle); }

	constexpr operator GLuint(void) const noexcept { return handle; }

protected:

	explicit shader(GLenum _spec)
		: handle(_tfunc<__impl_create_shader>::proc(_spec)) {}


	template<typename _InputIter>
	void __compile_source(_InputIter _begin, _InputIter _end, std::error_code & _ec)
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
		_tfunc<__impl_shader_source>::proc(handle,
			_src_cnt, _data_arr.data(), _length_arr.data());
		this->__compile(_ec);
	}

	void __compile_source(string_view const & _src, std::error_code & _ec)
	{
		GLint src_size = _src.size();
		GLchar const * src_data = _src.data();
		_tfunc<__impl_shader_source>::proc(handle, 1, &src_data, &src_size);
		this->__compile(_ec);
	}

	void __compile(std::error_code & _ec)
	{
		_tfunc<__impl_compile_shader>::proc(handle);
		GLint is_compiled{};
		_tfunc<__impl_get_shader_iv>::proc(handle, GL_COMPILE_STATUS, &is_compiled);
		if (!is_compiled) _ec = error::shader_compilation_failure;
	}

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

	program & link(std::initializer_list<GLuint> _arr, std::error_code & _ec)
	{
		return this->link(_arr.begin(), _arr.end(), _ec);
	}

	program & link(std::initializer_list<GLuint> _arr)
	{
		return this->link(_arr.begin(), _arr.end());
	}

	template<typename _Logbuf>
	program & get_log(_Logbuf & _log)
	{
		GLint log_length{};
		_tfunc<__impl_get_program_iv>::proc(handle, GL_INFO_LOG_LENGTH, &log_length);
		_tfunc<__impl_get_program_infolog>::proc(handle,
			log_length, &log_length, _log.get(log_length));
		return *this;
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

private:

	GLuint handle;
};

} // namespace gl
} // namespace vx

#endif // VX_GL_SHADER_HPP
