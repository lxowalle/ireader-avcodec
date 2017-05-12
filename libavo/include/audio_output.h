#ifndef _audio_output_h_
#define _audio_output_h_

#ifdef __cplusplus
extern "C"{
#endif

typedef struct
{
	/// @param[in] samples audio output buffer size(by sample)
	void* (*open)(int channels, int bits_per_sample, int samples_per_second, int samples);
	int (*close)(void* ao);
	
	/// @param[in] samples sample buffer
	/// @param[in] count in sample count=samples*channels*bits_per_sample/8
	/// @return >0-write samples, <0-error
	int (*write)(void* ao, const void* samples, int count);

	int (*play)(void* ao);
	int (*pause)(void* ao);
	int (*reset)(void* ao);

	/// @return sample number
	int (*get_samples)(void* ao);

	int (*set_volume)(void* ao, int v);
	int (*get_volume)(void* ao, int *v);
} audio_output_t;

void* audio_output_open(int channels, int bits_per_samples, int samples_per_seconds, int samples);
int audio_output_close(void* ao);

int audio_output_write(void* ao, const void* samples, int count);

int audio_output_play(void* ao);
int audio_output_pause(void* ao);
int audio_output_reset(void* ao);

/// @return available sample number
int audio_output_getsamples(void* ao);

int audio_output_setvolume(void* ao, int v);
int audio_output_getvolume(void* ao, int *v);

#ifdef __cplusplus
}

class audio_output
{
public:
	audio_output():m_ao(0), m_channels(0), m_bits_per_sample(0), m_samples_per_second(0){}
	~audio_output(){ close(); }

public:
	bool open(int channels, int bits_per_sample, int samples_per_second, int samples=0)
	{
		if(isopened() && check(channels, bits_per_sample, samples_per_second))
			return true;

		close();

		m_ao = audio_output_open(channels, bits_per_sample, samples_per_second, samples?samples:samples_per_second);
		m_channels = channels;
		m_bits_per_sample = bits_per_sample;
		m_samples_per_second = samples_per_second;
		return !!m_ao;
	}

	int write(const void* samples, int count){ return isopened()?audio_output_write(m_ao, samples, count) : -1; }

	bool isopened() const	{ return !!m_ao; }
	int close()				{ if(m_ao) audio_output_close(m_ao); m_ao = 0; return 0; }

	int play()				{ return isopened()?audio_output_play(m_ao)  : -1; }
	int pause()				{ return isopened()?audio_output_pause(m_ao) : -1; }
	int reset()				{ return isopened()?audio_output_reset(m_ao) : -1; }

	int getvolume(int &v) const	{ return isopened()?audio_output_getvolume(m_ao, &v) : -1; }
	int setvolume(int v)	{ return isopened()?audio_output_setvolume(m_ao, v) : -1; }

	int getsample() const	{ return isopened()?audio_output_getsamples(m_ao) : -1; }

private:
	bool check(int channels, int bits_per_sample, int samples_per_second)
	{
		if(!m_ao) return false;
		return m_channels==channels && m_bits_per_sample==bits_per_sample && m_samples_per_second==samples_per_second;
	}
	
private:
	audio_output(const audio_output&){}
	audio_output& operator=(const audio_output&){ return *this; }

private:
	void* m_ao;
	int m_channels;
	int m_bits_per_sample;
	int m_samples_per_second;
};

#endif

#endif /* !_audio_output_h_ */