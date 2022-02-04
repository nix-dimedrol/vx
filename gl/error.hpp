#ifndef VX_GL_ERROR_HPP
#define VX_GL_ERROR_HPP

#include <system_error>

namespace vx
{

namespace errc
{

struct graphics_error_category : std::error_category
{
	virtual char const * name(void) const noexcept override {
		return "graphics"; }
	virtual std::string message(int) const override;
};

} // namespace errc

namespace detail
{

auto& __graphics_category(void)
{
	static errc::graphics_error_category _instance{};
	return _instance;
}

} // namespace detail

namespace gl
{
namespace error
{

enum basic_errors
{
	invalid_context_version = 0x10,
	shader_compilation_failure,
	program_link_failure
};

std::error_code make_error_code(basic_errors _e)
{
	return {static_cast<int>(_e), vx::detail::__graphics_category()};
}

} // namespace error
} // namespace gl
} // namespace vx


namespace std
{

template<> struct is_error_code_enum<vx::gl::error::basic_errors>
	: true_type {};

} // namespace std



std::string vx::errc::graphics_error_category::message(int _ev) const
{
	switch (_ev)
	{
	case gl::error::invalid_context_version:
		return "Invalid GL context version";
	case gl::error::shader_compilation_failure:
		return "Fail to compile GL shader";
	case gl::error::program_link_failure:
		return "Fail to link GL program";
	default:
		return "Unknown error";
	}
}


#endif // VX_GL_ERROR_HPP
