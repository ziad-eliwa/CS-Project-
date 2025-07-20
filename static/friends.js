// Friends page JavaScript - Basic UI interactions (no backend functionality)

document.addEventListener("DOMContentLoaded", function () {
  // Tab switching functionality
  const tabButtons = document.querySelectorAll(".tab-btn");
  const tabContents = document.querySelectorAll(".tab-content");

  tabButtons.forEach((button) => {
    button.addEventListener("click", function () {
      const targetTab = this.getAttribute("data-tab");

      // Remove active class from all tabs and contents
      tabButtons.forEach((btn) => btn.classList.remove("active"));
      tabContents.forEach((content) => content.classList.remove("active"));

      // Add active class to clicked tab and corresponding content
      this.classList.add("active");
      document.getElementById(targetTab).classList.add("active");
    });
  });

  // Friends search functionality
  const friendsSearchInput = document.querySelector(".friends-search-input");
  const filterSelect = document.querySelector(".filter-select");

  if (friendsSearchInput) {
    friendsSearchInput.addEventListener("input", function () {
      const searchTerm = this.value.toLowerCase();
      const friendCards = document.querySelectorAll(".friend-card");

      friendCards.forEach((card) => {
        const friendName = card.querySelector("h3").textContent.toLowerCase();
        if (friendName.includes(searchTerm)) {
          card.style.display = "block";
        } else {
          card.style.display = "none";
        }
      });
    });
  }

  // Filter friends functionality
  if (filterSelect) {
    filterSelect.addEventListener("change", function () {
      const filterValue = this.value;
      const friendCards = document.querySelectorAll(".friend-card");

      friendCards.forEach((card) => {
        const onlineStatus = card.querySelector(".online-status");
        const isOnline =
          onlineStatus && onlineStatus.classList.contains("online");

        switch (filterValue) {
          case "online":
            card.style.display = isOnline ? "block" : "none";
            break;
          case "all":
          default:
            card.style.display = "block";
            break;
        }
      });
    });
  }

  // Friend request actions
  const acceptRequestButtons = document.querySelectorAll(".accept-request-btn");
  const declineRequestButtons = document.querySelectorAll(
    ".decline-request-btn"
  );
  const cancelRequestButtons = document.querySelectorAll(".cancel-request-btn");

  acceptRequestButtons.forEach((button) => {
    button.addEventListener("click", function () {
      const requestCard = this.closest(".request-card");
      const requestInfo = requestCard.querySelector("h4").textContent;

      // Animate the acceptance
      this.innerHTML = '<i class="fas fa-check"></i> Accepted';
      this.style.background = "#42b883";
      this.disabled = true;

      // Hide decline button
      const declineBtn = requestCard.querySelector(".decline-request-btn");
      if (declineBtn) {
        declineBtn.style.display = "none";
      }

      // Update badge count
      updateRequestCount(-1);

      // Remove card after animation
      setTimeout(() => {
        requestCard.style.opacity = "0";
        requestCard.style.transform = "translateY(-20px)";

        setTimeout(() => {
          requestCard.remove();
        }, 300);
      }, 1000);

      // Show notification
      showNotification(`You are now friends with ${requestInfo}!`, "success");
    });
  });

  declineRequestButtons.forEach((button) => {
    button.addEventListener("click", function () {
      const requestCard = this.closest(".request-card");
      const requestInfo = requestCard.querySelector("h4").textContent;

      // Animate the decline
      this.innerHTML = '<i class="fas fa-times"></i> Declined';
      this.style.background = "#e74c3c";
      this.disabled = true;

      // Hide accept button
      const acceptBtn = requestCard.querySelector(".accept-request-btn");
      if (acceptBtn) {
        acceptBtn.style.display = "none";
      }

      // Update badge count
      updateRequestCount(-1);

      // Remove card after animation
      setTimeout(() => {
        requestCard.style.opacity = "0";
        requestCard.style.transform = "translateY(-20px)";

        setTimeout(() => {
          requestCard.remove();
        }, 300);
      }, 1000);

      // Show notification
      showNotification(`Friend request from ${requestInfo} declined.`, "info");
    });
  });

  cancelRequestButtons.forEach((button) => {
    button.addEventListener("click", function () {
      const requestCard = this.closest(".request-card");
      const requestInfo = requestCard.querySelector("h4").textContent;

      // Animate the cancellation
      this.innerHTML = '<i class="fas fa-check"></i> Cancelled';
      this.disabled = true;

      // Remove card after animation
      setTimeout(() => {
        requestCard.style.opacity = "0";
        requestCard.style.transform = "translateY(-20px)";

        setTimeout(() => {
          requestCard.remove();
        }, 300);
      }, 1000);

      // Show notification
      showNotification(`Friend request to ${requestInfo} cancelled.`, "info");
    });
  });

  // Add friend suggestions
  const addFriendSuggestionButtons = document.querySelectorAll(
    ".add-friend-suggestion-btn"
  );
  const removeSuggestionButtons = document.querySelectorAll(
    ".remove-suggestion-btn"
  );

  addFriendSuggestionButtons.forEach((button) => {
    button.addEventListener("click", function () {
      const suggestionCard = this.closest(".suggestion-card");
      const suggestionInfo = suggestionCard.querySelector("h4").textContent;

      this.innerHTML = '<i class="fas fa-check"></i> Request Sent';
      this.style.background = "#42b883";
      this.disabled = true;

      // Show notification
      showNotification(`Friend request sent to ${suggestionInfo}!`, "success");
    });
  });

  removeSuggestionButtons.forEach((button) => {
    button.addEventListener("click", function () {
      const suggestionCard = this.closest(".suggestion-card");

      suggestionCard.style.opacity = "0";
      suggestionCard.style.transform = "scale(0.8)";

      setTimeout(() => {
        suggestionCard.remove();
      }, 300);
    });
  });

  // Message button functionality
  const messageButtons = document.querySelectorAll(".message-btn");
  messageButtons.forEach((button) => {
    button.addEventListener("click", function () {
      const friendCard = this.closest(".friend-card");
      const friendName = friendCard.querySelector("h3").textContent;

      // Simulate opening a message
      showNotification(`Opening chat with ${friendName}...`, "info");
    });
  });

  // Advanced search functionality
  const searchFriendsBtn = document.querySelector(".search-friends-btn");
  const searchFilterInputs = document.querySelectorAll(".search-filter-input");

  if (searchFriendsBtn) {
    searchFriendsBtn.addEventListener("click", function () {
      const searchValues = {};

      searchFilterInputs.forEach((input, index) => {
        const labels = ["name", "location", "school", "workplace"];
        searchValues[labels[index]] = input.value.trim();
      });

      // Simulate search
      this.innerHTML = '<i class="fas fa-spinner fa-spin"></i> Searching...';
      this.disabled = true;

      setTimeout(() => {
        this.innerHTML = '<i class="fas fa-search"></i> Search Friends';
        this.disabled = false;

        // Show mock results
        const resultsGrid = document.querySelector(".search-results-grid");
        if (
          resultsGrid &&
          Object.values(searchValues).some((val) => val !== "")
        ) {
          resultsGrid.style.display = "grid";
          showNotification(
            "Search completed! Found 2 potential friends.",
            "success"
          );
        } else {
          showNotification("Please enter search criteria.", "warning");
        }
      }, 2000);
    });
  }

  // Add friend from search results
  const addFriendResultButtons = document.querySelectorAll(
    ".add-friend-result-btn"
  );
  addFriendResultButtons.forEach((button) => {
    button.addEventListener("click", function () {
      const resultCard = this.closest(".search-result-card");
      const resultInfo = resultCard.querySelector("h5").textContent;

      this.innerHTML = '<i class="fas fa-check"></i> Request Sent';
      this.style.background = "#42b883";
      this.disabled = true;

      showNotification(`Friend request sent to ${resultInfo}!`, "success");
    });
  });

  // Helper functions
  function updateRequestCount(change) {
    const badge = document.querySelector(".tab-badge");
    if (badge) {
      const currentCount = parseInt(badge.textContent);
      const newCount = Math.max(0, currentCount + change);
      badge.textContent = newCount;

      if (newCount === 0) {
        badge.style.display = "none";
      }
    }

    // Update navbar notification badge
    const navBadge = document.querySelector(".notification-badge");
    if (navBadge) {
      const currentNavCount = parseInt(navBadge.textContent);
      const newNavCount = Math.max(0, currentNavCount + change);
      navBadge.textContent = newNavCount;

      if (newNavCount === 0) {
        navBadge.style.display = "none";
      }
    }
  }

  function showNotification(message, type = "info") {
    // Create notification element
    const notification = document.createElement("div");
    notification.className = `notification notification-${type}`;
    notification.innerHTML = `
            <i class="fas fa-${getNotificationIcon(type)}"></i>
            <span>${message}</span>
            <button class="notification-close">&times;</button>
        `;

    // Style the notification
    Object.assign(notification.style, {
      position: "fixed",
      top: "80px",
      right: "20px",
      background: getNotificationColor(type),
      color: "white",
      padding: "15px 20px",
      borderRadius: "8px",
      boxShadow: "0 4px 12px rgba(0, 0, 0, 0.2)",
      zIndex: "9999",
      display: "flex",
      alignItems: "center",
      gap: "10px",
      minWidth: "300px",
      transform: "translateX(100%)",
      transition: "transform 0.3s ease",
    });

    // Add to page
    document.body.appendChild(notification);

    // Animate in
    setTimeout(() => {
      notification.style.transform = "translateX(0)";
    }, 100);

    // Close button functionality
    const closeBtn = notification.querySelector(".notification-close");
    closeBtn.style.cssText = `
            background: none;
            border: none;
            color: white;
            font-size: 18px;
            cursor: pointer;
            margin-left: auto;
        `;

    closeBtn.addEventListener("click", () => {
      removeNotification(notification);
    });

    // Auto remove after 5 seconds
    setTimeout(() => {
      removeNotification(notification);
    }, 5000);
  }

  function removeNotification(notification) {
    notification.style.transform = "translateX(100%)";
    setTimeout(() => {
      if (notification.parentNode) {
        notification.parentNode.removeChild(notification);
      }
    }, 300);
  }

  function getNotificationIcon(type) {
    switch (type) {
      case "success":
        return "check-circle";
      case "warning":
        return "exclamation-triangle";
      case "error":
        return "times-circle";
      default:
        return "info-circle";
    }
  }

  function getNotificationColor(type) {
    switch (type) {
      case "success":
        return "#42b883";
      case "warning":
        return "#f39c12";
      case "error":
        return "#e74c3c";
      default:
        return "#0e4bf1";
    }
  }

  // Navigation hover effects
  const navIcons = document.querySelectorAll(".nav-icon");
  navIcons.forEach((icon) => {
    icon.addEventListener("mouseenter", function () {
      this.style.transform = "scale(1.1)";
    });

    icon.addEventListener("mouseleave", function () {
      this.style.transform = "scale(1)";
    });
  });

  // Card hover animations
  const cards = document.querySelectorAll(
    ".friend-card, .request-card, .suggestion-card, .search-result-card"
  );
  cards.forEach((card) => {
    card.addEventListener("mouseenter", function () {
      this.style.transform = "translateY(-3px)";
    });

    card.addEventListener("mouseleave", function () {
      this.style.transform = "translateY(0)";
    });
  });
});
