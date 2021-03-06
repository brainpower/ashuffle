package workspace

import (
	"errors"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
)

// Workspace represents a directory controlled by the application, that can
// be used for building software.
type Workspace struct {
	// Root is the (absolute) path to the root of the workspace.
	Root string

	prevDir string
	cleaned bool
}

// Path returns the absolute path to the given path, in this workspace.
func (w *Workspace) Path(rel ...string) string {
	paths := append([]string{w.Root}, rel...)
	return filepath.Join(paths...)
}

// Cleanup should be called when the workspace is no longer needed. It deletes
// all files in the workspace, as well as the workspace root itself.
func (w *Workspace) Cleanup() error {
	if err := os.Chdir(w.prevDir); err != nil {
		log.Printf("Unable to move to previous dir %q: %v", w.prevDir, err)
	}
	log.Printf("CD %q", w.prevDir)
	if w.cleaned {
		return errors.New("workspace already cleaned up")
	}
	w.cleaned = true
	return os.RemoveAll(w.Root)
}

type options struct {
	skipCD bool
}

type Option func(*options)

func NoCD(o *options) {
	o.skipCD = true
}

// New creates and returns a new workspace. The working directory is moved to
// new workspace when it is created, and moved back to the original
// working directory when Workspace.Cleanup() is called.
func New(opts ...Option) (*Workspace, error) {
	var wopts options
	for _, o := range opts {
		o(&wopts)
	}

	path, err := ioutil.TempDir("", "workspace-*")
	if err != nil {
		return nil, err
	}

	path, err = filepath.Abs(path)
	if err != nil {
		os.RemoveAll(path)
		return nil, err
	}

	cwd, err := os.Getwd()
	if err != nil {
		os.RemoveAll(path)
		return nil, err
	}

	if !wopts.skipCD {
		if err := os.Chdir(path); err != nil {
			os.RemoveAll(path)
			return nil, err
		}
		log.Printf("CD %q", path)
	}

	return &Workspace{Root: path, prevDir: cwd}, nil
}
