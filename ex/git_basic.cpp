#include "git2.h"
#include <stdio.h>
#include <cstring>

static int fetch_progress(const git_transfer_progress *stats, void *payload)
{
    int fetch_percent = (100 * stats->received_objects) / stats->total_objects;
    int index_percent = (100 * stats->indexed_objects) / stats->total_objects;
    int kbytes = stats->received_bytes / 1024;

    printf("network %3d%% (%4d kb, %5d/%5d)  /  index %3d%% (%5d/%5d)\n",
            fetch_percent, kbytes,
            stats->received_objects, stats->total_objects,
            index_percent,
            stats->indexed_objects, stats->total_objects);
    return 0;
}

static void show_branch(git_repository *repo)
{
  int error = 0;
  const char *branch = NULL;
  git_reference *head = NULL;

  error = git_repository_head(&head, repo);

  if (error == GIT_EUNBORNBRANCH || error == GIT_ENOTFOUND)
    branch = NULL;
  else if (!error) {
    branch = git_reference_name(head);
    if (!strncmp(branch, "refs/heads/", strlen("refs/heads/")))
      branch += strlen("refs/heads/");
  }
  else
  {
        const git_error *error = giterr_last();
        printf("Error %d %s - %s\n", error, "show_branch", (error && error->message) ? error->message : "???");
  }

    printf("# On branch %s\n", branch ? branch : "Not currently on any branch.");

  git_reference_free(head);
}

static int do_clone(const char *url, const char *path)
{
    git_repository *repo = NULL;
    git_clone_options opts = GIT_CLONE_OPTIONS_INIT;
    int ret;

    opts.fetch_progress_cb = fetch_progress;
    ret = git_clone(&repo, url, path, &opts);
    
    if(ret == GIT_ERROR)
    {
        const git_error *error = giterr_last();
        printf("Error %d %s - %s\n", ret, "clone", (error && error->message) ? error->message : "???");
    }
    
    show_branch(repo);
    git_repository_free(repo);
    
    return ret;
}

/*
 load a repo
git_repository *repo;
int error = git_repository_open(
  &repo,
  "/path/to/repository/on/disk");

init new
git_repository *repo;
int error = git_repository_init(
  &repo,                // output
  "path/to/new/repo",   // path
  false);               // bare?

Clone and get branch name above.
Still Need
    branch
    merge
    commit
    push
    pull

*/

int main(int argc, char **argv)
{
    const char *url, *path;

    if (argc < 3) {
        printf("USAGE: clone <url> <path>\n");
        return -1;
    }

    url = argv[1];
    path = argv[2];
    return do_clone(url, path);
}
