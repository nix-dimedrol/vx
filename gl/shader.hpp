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


struct program;

struct shader : noncopyable
{
	template<typename _Logbuf>
	explicit shader(GLenum _spec, string_view const & _src, _Logbuf & _log)
		: _M_handler(_tfunc<__impl_create_shader>::proc(_spec))
	{
		GLint src_size = _src.size();
		GLchar const * src_data = _src.data();
		_tfunc<__impl_shader_source>::proc(_M_handler, 1, &src_data, &src_size);
		_tfunc<__impl_compile_shader>::proc(_M_handler);
		GLint is_compiled{};
		_tfunc<__impl_get_shader_iv>::proc(_M_handler, GL_COMPILE_STATUS, &is_compiled);
		if (is_compiled == GL_TRUE) return;
		GLint log_length{};
		_tfunc<__impl_get_shader_iv>::proc(_M_handler, GL_INFO_LOG_LENGTH, &log_length);
		_tfunc<__impl_get_shader_infolog>::proc(_M_handler, log_length, &log_length, _log.get(log_length));
		_tfunc<__impl_delete_shader>::proc(_M_handler);
		_M_handler = 0;
	}

	explicit shader(shader && _other)
	{
		_M_handler = _other._M_handler;
		_other._M_handler = 0;
	}

	shader & operator= (shader && _other)
	{
		_M_handler = _other._M_handler;
		_other._M_handler = 0;
		return *this;
	}

	constexpr explicit operator bool (void) noexcept { return bool(_M_handler); }
	constexpr bool operator! (void) noexcept { return !static_cast<bool>(*this); }

	~shader(void) { if (_M_handler) _tfunc<__impl_delete_shader>::proc(_M_handler); }

protected:

	GLuint get_handler(void) const { return _M_handler; }

private:

	GLuint _M_handler;

	friend program;
};


struct program : noncopyable
{
	explicit program(void) : _M_handler{} {}

	template<typename _Logbug, typename... _Args, typename = std::enable_if_t<
		types_pack<shader, std::remove_reference_t<std::remove_const_t<_Args>>...>::is_same::value>>
	explicit program(_Logbug & _log, _Args const &&... _shaders)
		: _M_handler(_tfunc<__impl_create_program>::proc())
	{
		(_tfunc<__impl_attach_shader>::proc(_M_handler, _shaders.get_handler()), ...);
		_tfunc<__impl_link_program>::proc(_M_handler);
		GLint is_linked{};
		_tfunc<__impl_get_program_iv>::proc(_M_handler, GL_LINK_STATUS, &is_linked);
		if (is_linked == GL_TRUE)
		{
			(_tfunc<__impl_detach_shader>::proc(_M_handler, _shaders.get_handler()), ...);
			return;
		}
		GLint log_length{};
		_tfunc<__impl_get_program_iv>::proc(_M_handler, GL_INFO_LOG_LENGTH, &log_length);
		_tfunc<__impl_get_program_infolog>::proc(_M_handler, log_length, &log_length, _log.get(log_length));
		_tfunc<__impl_delete_program>::proc(_M_handler);
		_M_handler = 0;
	}

	explicit program(program && _other)
	{
		_M_handler = _other._M_handler;
		_other._M_handler = 0;
	}

	program & operator= (program && _other)
	{
		_M_handler = _other._M_handler;
		_other._M_handler = 0;
		return *this;
	}

	void use(void) { _tfunc<__impl_use_program>::proc(_M_handler); }

	GLint get_uniform_location(string_view const & _name)
	{ return _tfunc<__impl_get_uniform_location>::proc(_M_handler, _name.data()); }

	constexpr explicit operator bool (void) noexcept { return bool(_M_handler); }
	constexpr bool operator! (void) noexcept { return !static_cast<bool>(*this); }

	~program(void) { if (_M_handler) _tfunc<__impl_delete_program>::proc(_M_handler); }

private:

	GLuint _M_handler;
};

} // namespace gl
} // namespace vx

#endif // VX_GL_SHADER_HPP
