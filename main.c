#include <sys/soundcard.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

static int verbose=0;
static int dspfd=-1;
static char const *dspdev="/dev/dsp";
static char const *me="dsprec";

void dspopen ()
{
  if (dspfd < 0) {
    dspfd = open(dspdev, O_RDWR);
    if (dspfd < 0) {
      fprintf (stderr, "%s: can't open %s: %s\n", me, dspdev, strerror(errno));
      exit (1);
    }
  }
}

void setrate (int newrate)
{
  dspopen();
  if (ioctl (dspfd, SNDCTL_DSP_SPEED, &newrate) < 0) {
    fprintf (stderr, "%s: setrate(%d): %s\n", me, newrate, strerror(errno));
  }
}

void setchannels (int channels)
{
  dspopen();
  if (ioctl (dspfd, SNDCTL_DSP_CHANNELS, &channels) < 0) {
    fprintf (stderr, "%s: setchannels(%d): %s\n", me, channels, strerror(errno));
  }
}

void cat ()
{
  char buf[4410];
  int n;

  dspopen();
  while ((n=read (dspfd, buf, sizeof(buf))))
    write (STDOUT_FILENO, buf, n);
}

void setbits (int bits)
{
  int fmt;
  if (bits == 8)
    fmt = AFMT_S8;
  else if (bits == 16)
    fmt = AFMT_S16_LE;
  else {
    fprintf (stderr, "%s: setbits(%d): unrecognized word size\n", me, bits);
    return;
  }
  dspopen();
  if (ioctl (dspfd, SNDCTL_DSP_SETFMT, &fmt) < 0) {
    fprintf (stderr, "%s: setbits(%d): %s\n", me, bits, strerror(errno));
  }
}

int main(int argc, char** argv)
{
  int a;
  me=argv[0];
  for(a=1; a<argc; a++) {
    if (!strcmp (argv[a], "-v"))
      verbose=1;
    else if(a+1<argc && !strcmp (argv[a], "-f")) {
      if (dspfd >= 0)
	close (dspfd);
      dspfd = -1;
      dspdev = argv[++a];
    }
    else if(a+1<argc && !strcmp (argv[a], "rate"))
      setrate (atoi (argv[++a]));
    else if(a+1<argc && !strcmp (argv[a], "bits"))
      setbits (atoi (argv[++a]));
    else if(a+1<argc && !strcmp (argv[a], "channels"))
      setchannels (atoi (argv[++a]));
    else
      fprintf(stderr, "%s: unrecognized command line option: %s\n",
	      argv[0], argv[a]);
  }
  cat();
  exit(0);
}
