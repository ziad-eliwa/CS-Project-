// Timeline page JavaScript - Basic UI interactions (no backend functionality)

document.addEventListener("DOMContentLoaded", function () {
  // Fetch and render posts from backend
  function loadPosts() {
    fetch('/api/posts')
      .then(response => response.json())
      .then(posts => {
        const postsFeed = document.querySelector('.posts-feed');
        postsFeed.innerHTML = '';
        posts.forEach(post => {
          const newPost = document.createElement('article');
          newPost.className = 'post';
          newPost.innerHTML = `
            <div class="post-header">
              <img src="https://via.placeholder.com/50" alt="User" class="post-avatar">
              <div class="post-user-info">
                <h4>${post.user_name}</h4>
                <p class="post-time">${post.timestamp || ''}</p>
              </div>
              <div class="post-options">
                <i class="fas fa-ellipsis-h"></i>
              </div>
            </div>
            <div class="post-content">
              <p>${post.content}</p>
            </div>
            <div class="post-stats">
              <span class="likes-count">👍 0 likes</span>
              <span class="comments-count">💬 0 comments</span>
              <span class="shares-count">🔄 0 shares</span>
            </div>
            <div class="post-actions">
              <button class="action-btn like-btn">
                <i class="far fa-thumbs-up"></i> Like
              </button>
              <button class="action-btn comment-btn">
                <i class="far fa-comment"></i> Comment
              </button>
              <button class="action-btn share-btn">
                <i class="fas fa-share"></i> Share
              </button>
            </div>
            <div class="comments-section" style="display:none;">
              <div class="write-comment">
                <img src="https://via.placeholder.com/35" alt="Your avatar" class="comment-avatar">
                <input type="text" placeholder="Write a comment..." class="comment-input">
                <button class="send-comment-btn">
                  <i class="fas fa-paper-plane"></i>
                </button>
              </div>
              <div class="comments-list"></div>
            </div>
          `;
          postsFeed.appendChild(newPost);
          attachPostEventListeners(newPost);
        });
      });
  }

  // Helper to get username from sessionStorage or cookie
  function getCurrentUsername() {
    let username = sessionStorage.getItem('username');
    if (username) return username;
    const match = document.cookie.match(/username=([^;]+)/);
    if (match) return match[1];
    return '';
  }

  // Create post functionality (send to backend)
  const createPostInput = document.querySelector(".create-post-input");
  const postButton = document.querySelector(".post-btn");

  if (postButton) {
    postButton.addEventListener("click", function () {
      const postContent = createPostInput.value.trim();
      const userName = getCurrentUsername();
      if (!userName) {
        alert('No username found. Please log in again.');
        return;
      }
      if (postContent) {
        fetch('/api/posts', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ user_name: userName, content: postContent })
        })
        .then(response => {
          if (response.ok) {
            createPostInput.value = '';
            loadPosts(); // Reload posts after successful creation
          } else {
            response.text().then(text => alert('Failed to create post: ' + text));
          }
        });
      }
    });
  }

  // Attach event listeners to post actions (like, comment, etc.)
  function attachPostEventListeners(post) {
    const likeBtn = post.querySelector(".like-btn");
    const commentBtn = post.querySelector(".comment-btn");
    const sendCommentBtn = post.querySelector(".send-comment-btn");
    const commentInput = post.querySelector(".comment-input");
    const commentsSection = post.querySelector(".comments-section");

    likeBtn.addEventListener("click", function () {
      this.classList.toggle("liked");
      if (this.classList.contains("liked")) {
        this.innerHTML = '<i class="fas fa-thumbs-up"></i> Liked';
      } else {
        this.innerHTML = '<i class="far fa-thumbs-up"></i> Like';
      }
    });

    commentBtn.addEventListener("click", function () {
      if (commentsSection.style.display === "none" || commentsSection.style.display === "") {
        commentsSection.style.display = "block";
      } else {
        commentsSection.style.display = "none";
      }
      commentInput.focus();
    });

    sendCommentBtn.addEventListener("click", function () {
      const commentText = commentInput.value.trim();
      if (commentText) {
        const commentsList = post.querySelector(".comments-list");
        const newComment = document.createElement("div");
        newComment.className = "comment";
        newComment.innerHTML = `
          <img src="https://via.placeholder.com/35" alt="Your avatar" class="comment-avatar">
          <div class="comment-content">
            <div class="comment-bubble">
              <strong>You</strong>
              <p>${commentText}</p>
            </div>
            <div class="comment-actions">
              <span class="comment-time">Just now</span>
              <button class="comment-like">Like</button>
              <button class="comment-reply">Reply</button>
            </div>
          </div>
        `;
        commentsList.appendChild(newComment);
        commentInput.value = "";
      }
    });

    commentInput.addEventListener("keypress", function (e) {
      if (e.key === "Enter") {
        sendCommentBtn.click();
      }
    });
  }

  // Initial load
  loadPosts();

  // Like button interactions
  const likeButtons = document.querySelectorAll(".like-btn");
  likeButtons.forEach((button) => {
    button.addEventListener("click", function () {
      this.classList.toggle("liked");
      if (this.classList.contains("liked")) {
        this.innerHTML = '<i class="fas fa-thumbs-up"></i> Liked';
      } else {
        this.innerHTML = '<i class="far fa-thumbs-up"></i> Like';
      }
    });
  });

  // Comment button interactions
  const commentButtons = document.querySelectorAll(".comment-btn");
  commentButtons.forEach((button) => {
    button.addEventListener("click", function () {
      const post = this.closest(".post");
      const commentsSection = post.querySelector(".comments-section");
      const commentInput = commentsSection.querySelector(".comment-input");

      // Toggle comments visibility and focus input
      if (commentsSection.style.display === "none") {
        commentsSection.style.display = "block";
      }
      commentInput.focus();
    });
  });

  // Send comment functionality
  const sendCommentButtons = document.querySelectorAll(".send-comment-btn");
  sendCommentButtons.forEach((button) => {
    button.addEventListener("click", function () {
      const commentInput = this.parentElement.querySelector(".comment-input");
      const commentText = commentInput.value.trim();

      if (commentText) {
        // Create new comment element
        const commentsList =
          this.closest(".comments-section").querySelector(".comments-list");
        const newComment = document.createElement("div");
        newComment.className = "comment";
        newComment.innerHTML = `
                    <img src="https://via.placeholder.com/35" alt="Your avatar" class="comment-avatar">
                    <div class="comment-content">
                        <div class="comment-bubble">
                            <strong>You</strong>
                            <p>${commentText}</p>
                        </div>
                        <div class="comment-actions">
                            <span class="comment-time">Just now</span>
                            <button class="comment-like">Like</button>
                            <button class="comment-reply">Reply</button>
                        </div>
                    </div>
                `;

        commentsList.appendChild(newComment);
        commentInput.value = "";
      }
    });
  });

  // Enter key for comments
  const commentInputs = document.querySelectorAll(".comment-input");
  commentInputs.forEach((input) => {
    input.addEventListener("keypress", function (e) {
      if (e.key === "Enter") {
        const sendButton =
          this.parentElement.querySelector(".send-comment-btn");
        sendButton.click();
      }
    });
  });

  // Load more posts
  const loadMoreBtn = document.querySelector(".load-more-btn");
  if (loadMoreBtn) {
    loadMoreBtn.addEventListener("click", function () {
      // Simulate loading more posts
      this.innerHTML = "Loading...";

      setTimeout(() => {
        this.innerHTML = "Load More Posts";
        // In a real app, you would fetch more posts from the server here
        console.log("Loading more posts...");
      }, 1000);
    });
  }

  // Friend request actions (right sidebar)
  const acceptButtons = document.querySelectorAll(".accept-btn");
  const declineButtons = document.querySelectorAll(".decline-btn");
  const addFriendButtons = document.querySelectorAll(".add-friend-btn");

  acceptButtons.forEach((button) => {
    button.addEventListener("click", function () {
      const requestCard = this.closest(".friend-request");
      requestCard.style.opacity = "0.5";
      this.innerHTML = "Accepted";
      this.disabled = true;

      setTimeout(() => {
        requestCard.remove();
      }, 1000);
    });
  });

  declineButtons.forEach((button) => {
    button.addEventListener("click", function () {
      const requestCard = this.closest(".friend-request");
      requestCard.style.opacity = "0.5";
      this.innerHTML = "Declined";
      this.disabled = true;

      setTimeout(() => {
        requestCard.remove();
      }, 1000);
    });
  });

  addFriendButtons.forEach((button) => {
    button.addEventListener("click", function () {
      this.innerHTML = "Request Sent";
      this.disabled = true;
      this.style.background = "#42b883";
    });
  });

  // Navigation icon hover effects
  const navIcons = document.querySelectorAll(".nav-icon");
  navIcons.forEach((icon) => {
    icon.addEventListener("mouseenter", function () {
      this.style.transform = "scale(1.1)";
    });

    icon.addEventListener("mouseleave", function () {
      this.style.transform = "scale(1)";
    });
  });
});
