#ifndef VX_PULSER_HPP
#define VX_PULSER_HPP

#include <atomic>
#include <chrono>


namespace vx
{

using std::size_t;


template<typename _Derived, size_t _Fps>
struct basic_pulser
{
	using derived_type = _Derived;

	static constexpr size_t fps = _Fps;
	using clock_type          = std::chrono::steady_clock;
	using frame_duration_type = std::chrono::duration<clock_type::rep, std::ratio<1, fps>>;
	using duration_type       = std::common_type_t<clock_type::duration, frame_duration_type>;

	void run(void)
	{
		size_t frame_seq_id = 0;
		auto start_tp = clock_type::now();

		while (!_M_should_stop.load(std::memory_order_acquire))
		{
			static_cast<derived_type*>(this)->update(frame_seq_id);

			frame_seq_id++;
			auto frame_end_tp = start_tp +
				std::chrono::duration<clock_type::rep, std::ratio<1, fps>>(frame_seq_id);
			auto now_tp = clock_type::now();
			if (frame_end_tp > now_tp)
			{
				static_cast<derived_type*>(this)->process(frame_end_tp - now_tp);
			}
		}

		_M_should_stop.store(false, std::memory_order_relaxed);
	}

	void stop(void)
	{
		_M_should_stop.store(true, std::memory_order_release);
	}

private:

	std::atomic<bool> _M_should_stop{ false };
};

} // namespace vx


#endif

