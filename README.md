# libnbar

The __Non-Binary Archive Format__ lives at the very heart of many tools used by [Rainbow Linux](https://github.com/rainbow-linux). This simple archive format holds a pair of files together.

# Why

Many applications require a pair of files to function. Examples include:

- `zstd`-compressed files paired with the compression dictionary
- Checksum files paired with GPG keys
- Large block files paired with their checksums
- Software packages paierd with their signature files

These files are often distributed separately, which makes it require more actions to obtain. The NBAR format tries to resolve this issue mainly in the context of the [iris](https://github.om/rainbow-linux/iris-pm) package manager, but it could be used for other applications that need pairs of files to function.