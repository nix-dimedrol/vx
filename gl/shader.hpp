#ifndef VX_GL_SHADER_HPP
#define VX_GL_SHADER_HPP


#include "common.hpp"


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


using __program_types_pack = types_pack<__impl_create_program, __impl_delete_program,
	__impl_attach_shader, __impl_detach_shader, __impl_link_program, __impl_use_program,
	__impl_get_program_iv, __impl_get_program_infolog, __impl_get_uniform_location>;


template<typename _Predicate>
void load_shader_procs(_Predicate _pred)
{
	__load_procs(_pred, __shader_types_pack{});
	__load_procs(_pred, __program_types_pack{});
}


struct shader : noncopyable
{
	explicit shader(GLenum _spec, string_view const & _src)
		: handle(_tfunc<__impl_create_shader>::proc(_spec))
	{
		GLint src_size = _src.size();
		GLchar const * src_data = _src.data();
		_tfunc<__impl_shader_source>::proc(handle, 1, &src_data, &src_size);
		_tfunc<__impl_compile_shader>::proc(handle);
		GLint is_compiled{};
		_tfunc<__impl_get_shader_iv>::proc(handle, GL_COMPILE_STATUS, &is_compiled);
		if (!is_compiled) error = true;
	}

	template<typename _Logbuf>
	explicit shader(GLenum _spec, string_view const & _src, _Logbuf & _log)
		: shader(_spec, _src) { get_log(_log); }

	template<typename _Logbuf>
	void get_log(_Logbuf & _log)
	{
		GLint log_length{};
		_tfunc<__impl_get_shader_iv>::proc(handle, GL_INFO_LOG_LENGTH, &log_length);
		_tfunc<__impl_get_shader_infolog>::proc(handle, log_length, &log_length, _log.get(log_length));
	}

	~shader(void) { if (handle) _tfunc<__impl_delete_shader>::proc(handle); }

	constexpr explicit operator bool (void) const noexcept { return static_cast<bool>(handle); }
	constexpr bool operator! (void) const noexcept { return !static_cast<bool>(*this); }

	constexpr operator GLuint(void) const noexcept { return handle; }

private:

	GLuint handle;
	bool error = false;
};


struct program : noncopyable
{
	explicit program(void)
		: handle(_tfunc<__impl_create_program>::proc()) {}

	program & link(std::initializer_list<GLuint> _arr)
	{
		for (auto it : _arr) _tfunc<__impl_attach_shader>::proc(handle, it);
		_tfunc<__impl_link_program>::proc(handle);
		GLint is_linked{};
		_tfunc<__impl_get_program_iv>::proc(handle, GL_LINK_STATUS, &is_linked);
		if (!is_linked) error = true;
		for (auto it : _arr) _tfunc<__impl_detach_shader>::proc(handle, it);
		return *this;
	}

	template<typename _Logbuf>
	program & get_log(_Logbuf & _log)
	{
		GLint log_length{};
		_tfunc<__impl_get_program_iv>::proc(handle, GL_INFO_LOG_LENGTH, &log_length);
		_tfunc<__impl_get_program_infolog>::proc(handle, log_length, &log_length, _log.get(log_length));
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

	constexpr explicit operator bool (void) const noexcept { return static_cast<bool>(handle); }
	constexpr bool operator! (void) const noexcept { return !static_cast<bool>(*this); }

	constexpr operator GLuint(void) const noexcept { return handle; }

private:

	GLuint handle;
	bool error = false;
};

} // namespace gl
} // namespace vx

#endif // VX_GL_SHADER_HPP
